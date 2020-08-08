import java.io.*;
import java.math.BigInteger;
import java.net.InetAddress;
import java.net.Socket;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.cert.Certificate;

public class Client
{
    public static void main(String[] args)
    {
        try
        {
            // Step 1 (client):
            SecureRandom randomSecure = new SecureRandom();
            byte[] clientNonce = new byte[32];
            randomSecure.nextBytes(clientNonce);
            ByteArrayOutputStream clientMessageConcatenation = new ByteArrayOutputStream();
            clientMessageConcatenation.write(clientNonce);

            InetAddress host = InetAddress.getLocalHost();
            Socket outgoingSocket = new Socket(host.getHostName(), 4242);
            System.out.println("New connection established on port 4242.");
            ObjectOutputStream oos = new ObjectOutputStream(outgoingSocket.getOutputStream());
            oos.flush();
            ObjectInputStream ois = new ObjectInputStream(outgoingSocket.getInputStream());

            oos.write(clientNonce);
            oos.flush();
            System.out.println("Handshake Step 1:  client nonce sent.");

            // Step 2, client-side.
            Certificate serverCert = (Certificate)ois.readObject();
            BigInteger serverDHPublicKey = (BigInteger)ois.readObject();
            BigInteger serverSignedDHPublicKey = (BigInteger)ois.readObject();
            System.out.println("Handshake Step 2:  server cert, DH public key, and signed DH public key received.");

            clientMessageConcatenation.write(serverCert.getEncoded());
            clientMessageConcatenation.write(serverDHPublicKey.toByteArray());
            clientMessageConcatenation.write(serverSignedDHPublicKey.toByteArray());

            //Step 3 (client):
            Certificate clientCert = Helpers.generateCertificate("./src/CASignedClientCertificate.pem");
            Certificate caCert = Helpers.generateCertificate("./src/CAcertificate.pem");
            PublicKey clientRSAPublicKey = Helpers.getRSAPublicKey(clientCert);
            PrivateKey clientRSAPrivateKey = Helpers.getRSAPrivateKey("./src/clientPrivateKey.der");
            BigInteger clientDHPrivateKey = Helpers.generateDHPrivateKey();
            BigInteger clientDHPublicKey = Helpers.generateDHPublicKey(clientDHPrivateKey);
            BigInteger clientSignedDHPublicKey = Helpers.signPublicKey(clientDHPublicKey, clientRSAPrivateKey,
                    clientRSAPublicKey);
            clientMessageConcatenation.write(clientCert.getEncoded());
            clientMessageConcatenation.write(clientDHPublicKey.toByteArray());
            clientMessageConcatenation.write(clientSignedDHPublicKey.toByteArray());

            oos.writeObject(clientCert);
            oos.flush();
            oos.writeObject(clientDHPublicKey);
            oos.flush();
            oos.writeObject(clientSignedDHPublicKey);
            oos.flush();
            System.out.println("Handshake Step 3:  client cert, DH public key, and signed DH public key sent.");

            // Step 4, client side:
            boolean clientCAVerification = Helpers.verifyCASignature(serverCert, caCert);
            System.out.println("Handshake Step 4:  client can verify certificate: " + clientCAVerification);
            boolean clientLocalVerification = Helpers.verifyLocalSignature(serverDHPublicKey, serverSignedDHPublicKey,
                    Helpers.getRSAPublicKey(serverCert));
            System.out.println("Handshake Step 4:  client can verify signature:  " + clientLocalVerification);

            BigInteger clientSharedSecret = Helpers.generateSharedSecret(serverDHPublicKey,
                    clientDHPrivateKey);
            System.out.println("Handshake Step 4:  client shared secret generated.");

            // Step 5, client side:
            SecretKeys clientSecretKeys = Helpers.makeSecretKeys(clientNonce, clientSharedSecret);
            System.out.println("Handshake Step 5:  client secret keys generated.");

            // Step 6, client side:
            byte[] serverFinalHandshake = (byte[])ois.readObject();
            System.out.println("Handshake Step 6:  server-side messages and MAC received.");
            byte[] serverFinalHandShakeVerification = Helpers.handshakeHMAC(clientMessageConcatenation,
                    clientSecretKeys, "server");
            if (java.util.Arrays.equals(serverFinalHandshake, serverFinalHandShakeVerification))
            {
                System.out.println("Handshake Step 6:  server-side messages verified.");
            }
            else
            {
                throw new Exception("Incorrect server messages.");
            }

            // Step 7, client side:
            clientMessageConcatenation.write(serverFinalHandshake);
            byte[] clientFinalHandshake = Helpers.handshakeHMAC(clientMessageConcatenation, clientSecretKeys, "client");
            oos.writeObject(clientFinalHandshake);
            oos.flush();
            System.out.println("Handshake Step 7:  client-side messages and MAC key sent.");

            System.out.println("Handshake completed.");

            // Receiving file, client side:
            ByteArrayOutputStream fileCollector = new ByteArrayOutputStream();
            byte[] firstServerOutput = (byte[])ois.readObject();
            File firstEncryptedFile = new File("./src/FirstEncryptedFromServer.txt");
            FileOutputStream fos = new FileOutputStream(firstEncryptedFile);
            fos.write(firstServerOutput);
            fos.close();
            System.out.println("Total encrypted file from server received, and saved at " +
                    "./src/FirstEncryptedFromServer.txt.");
            fileCollector.write(Helpers.decryptForReceipt(firstServerOutput, clientSecretKeys, "server"));
            System.out.println("First half of encrypted file from server received.");

            byte[] secondServerOutput = (byte[])ois.readObject();
            File secondEncryptedFile = new File("./src/SecondEncryptedFromServer.txt");
            fos = new FileOutputStream(secondEncryptedFile);
            fos.write(secondServerOutput);
            fos.close();
            System.out.println("Total encrypted file from server received, and saved at " +
                    "./src/SecondEncryptedFromServer.txt.");
            fileCollector.write(Helpers.decryptForReceipt(secondServerOutput, clientSecretKeys, "server"));
            System.out.println("Second half of encrypted file from server received.");

            File fileOnClient = new File("./src/ReceivedFile.txt");
            fos = new FileOutputStream(fileOnClient);
            fos.write(fileCollector.toByteArray());
            fos.close();
            System.out.println("Encrypted file from server decrypted and saved at ./src/ReceivedFile.txt");

            String response = "File received.";
            byte[] clientResponse = Helpers.encryptForSending(response.getBytes(), clientSecretKeys, "client");
            oos.writeObject(clientResponse);
            oos.flush();
            System.out.println("Receipt message sent to server.");

            response = "exit";
            clientResponse = Helpers.encryptForSending(response.getBytes(), clientSecretKeys, "client");
            oos.writeObject(clientResponse);
            oos.flush();
            System.out.println("Exit message sent to server, shutting down client.");

            oos.close();
            ois.close();
            outgoingSocket.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
