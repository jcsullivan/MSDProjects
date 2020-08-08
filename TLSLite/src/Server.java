import java.io.*;
import java.math.BigInteger;
import java.net.ServerSocket;
import java.net.Socket;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.cert.Certificate;

//*****NOTE:
//Server will ONLY accept streams in the order as described in the TLS homework, summarized below:
//Step 1: Client: Nonce1 (32 bytes from a SecureRandom object)
//Step 2: Server: Server Certificate, DiffieHellman public key, Signed DiffieHellman public key (Sign[g^ks % N, Spriv])
//Step 3: Client: Client Certificate, DiffieHellman public key, Signed DiffieHellman public key (Sign[g^kc % N, Cpriv])
//Step 4: Client and server compute the shared secret here using DH
//Step 5: client and server derive 6 session keys from the shared secret. 2 each of bulk encryption keys, MAC keys,
//        IVs for CBC using HKDF
//Step 6: Server: MAC(all handshake messages so far, Server's MAC key)
//Step 7: Client: MAC(all handshake messages so far including the previous step, Client's MAC key).
//Step 8: Server sends file to client.
//Step 9: Client sends acknowledgement to server.
//Step 10: Client exits from server and server shuts down.

public class Server
{
    public static void main(String[] args)
    {
        try
        {
            ServerSocket server = new ServerSocket(4242);
            System.out.println("Server activated on port 4242.");
            while(true)
            {
                Socket incomingSocket = server.accept();
                System.out.println("New connection established on port 4242.");
                ObjectOutputStream oos = new ObjectOutputStream(incomingSocket.getOutputStream());
                oos.flush();
                ObjectInputStream ois = new ObjectInputStream(incomingSocket.getInputStream());
                int bufSize = ois.available();
                byte[] clientNonce = new byte[bufSize];
                ois.readFully(clientNonce);
                System.out.println("Handshake Step 1:  client nonce read.");

                // Step 2 (server):
                ByteArrayOutputStream serverMessageConcatenation = new ByteArrayOutputStream();
                serverMessageConcatenation.write(clientNonce);
                Certificate serverCert = Helpers.generateCertificate("./src/CASignedServerCertificate.pem");
                Certificate caCert = Helpers.generateCertificate("./src/CAcertificate.pem");
                PublicKey serverRSAPublicKey = Helpers.getRSAPublicKey(serverCert);
                PrivateKey serverRSAPrivateKey = Helpers.getRSAPrivateKey("./src/serverPrivateKey.der");
                BigInteger serverDHprivateKey = Helpers.generateDHPrivateKey();
                BigInteger serverDHpublicKey = Helpers.generateDHPublicKey(serverDHprivateKey);
                BigInteger serverSignedDHPublicKey = Helpers.signPublicKey(serverDHpublicKey, serverRSAPrivateKey,
                        serverRSAPublicKey);
                serverMessageConcatenation.write(serverCert.getEncoded());
                serverMessageConcatenation.write(serverDHpublicKey.toByteArray());
                serverMessageConcatenation.write(serverSignedDHPublicKey.toByteArray());

                oos.writeObject(serverCert);
                oos.flush();
                oos.writeObject(serverDHpublicKey);
                oos.flush();
                oos.writeObject(serverSignedDHPublicKey);
                oos.flush();
                System.out.println("Handshake Step 2:  server cert, DH public key, and signed DH public key sent.");

                // Step 3 (server):
                Certificate clientCert = (Certificate)ois.readObject();
                BigInteger clientDHPublicKey = (BigInteger)ois.readObject();
                BigInteger clientSignedDHPublicKey = (BigInteger)ois.readObject();
                System.out.println("Handshake Step 3:  client cert, DH public key, and signed DH public key received.");

                serverMessageConcatenation.write(clientCert.getEncoded());
                serverMessageConcatenation.write(clientDHPublicKey.toByteArray());
                serverMessageConcatenation.write(clientSignedDHPublicKey.toByteArray());

                // Step 4, server side:
                boolean serverCAVerification = Helpers.verifyCASignature(clientCert, caCert);
                System.out.println("Handshake Step 4:  server can verify certificate: " + serverCAVerification);
                boolean serverLocalVerification = Helpers.verifyLocalSignature(clientDHPublicKey,
                        clientSignedDHPublicKey, Helpers.getRSAPublicKey(clientCert));
                System.out.println("Handshake Step 4:  server can verify signature:  " + serverLocalVerification);

                BigInteger serverSharedSecret = Helpers.generateSharedSecret(clientDHPublicKey, serverDHprivateKey);
                System.out.println("Handshake Step 4:  server shared secret generated.");

                // Step 5, server side:
                SecretKeys serverSecretKeys = Helpers.makeSecretKeys(clientNonce, serverSharedSecret);
                System.out.println("Handshake Step 5:  server secret keys generated.");

                // Step 6, server side:
                byte[] serverFinalHandshake = Helpers.handshakeHMAC(serverMessageConcatenation, serverSecretKeys, "server");
                serverMessageConcatenation.write(serverFinalHandshake);
                oos.writeObject(serverFinalHandshake);
                oos.flush();
                System.out.println("Handshake Step 6:  server-side messages and MAC key sent.");

                // Step 7, server side:
                byte[] clientFinalHandshake = (byte[])ois.readObject();
                System.out.println("Handshake Step 7:  client-side messages and MAC received.");
                byte[] clientFinalHandshakeVerification = Helpers.handshakeHMAC(serverMessageConcatenation, serverSecretKeys,
                        "client");
                if (java.util.Arrays.equals(clientFinalHandshake, clientFinalHandshakeVerification))
                {
                    System.out.println("Handshake Step 7:  client-side messages verified.");
                }
                else
                {
                    throw new Exception("Incorrect client messages.");
                }
                System.out.println("Handshake completed.");

                // Sending file, server side:
                File fileOnServer = new File("./src/TheMoonisaHarshMistress.txt");
                byte[] fileByBytesFirst = new byte[((int)fileOnServer.length())/2];
                byte[] fileByBytesSecond = new byte[((int)fileOnServer.length())/2 + 1];
                FileInputStream fis = new FileInputStream(fileOnServer);
                fis.read(fileByBytesFirst);
                fis.read(fileByBytesSecond);
                fis.close();

                byte[] firstServerOutput = Helpers.encryptForSending(fileByBytesFirst, serverSecretKeys, "server");

                oos.writeObject(firstServerOutput);
                oos.flush();
                System.out.println("First half sent to client.");

                byte[] secondServerOutput = Helpers.encryptForSending(fileByBytesSecond, serverSecretKeys, "server");

                oos.writeObject(secondServerOutput);
                oos.flush();
                System.out.println("Second half sent to client.");

                byte[] clientEncryptedResponse = (byte[])ois.readObject();
                byte[] clientResponse = Helpers.decryptForReceipt(clientEncryptedResponse, serverSecretKeys,
                        "client");
                System.out.println("Response from client:  " + new String(clientResponse));

                byte[] clientEncryptedFinal = (byte[])ois.readObject();
                byte[] clientFinal = Helpers.decryptForReceipt(clientEncryptedFinal, serverSecretKeys, "client");

                if(new String(clientFinal).equals("exit"))
                {
                    System.out.println("Client has sent exit command.  Shutting down server.");
                    oos.close();
                    ois.close();
                    incomingSocket.close();
                    break;
                }
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

    }
}
