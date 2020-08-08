import javax.crypto.Cipher;
import javax.crypto.Mac;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.io.*;
import java.math.BigInteger;
import java.security.*;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.Arrays;
import java.util.Date;
import java.util.Random;

public class Helpers
{
    // Takes in a file path, returns the certificate found at that file, if any.  Throws exception if cannot.
    public static Certificate generateCertificate(String filePath) throws Exception
    {
        FileInputStream fis = new FileInputStream(filePath);
        BufferedInputStream bis = new BufferedInputStream(fis);

        CertificateFactory cf = CertificateFactory.getInstance("X.509");

        Certificate cert = null;
        while(bis.available() > 0)
        {
            cert = cf.generateCertificate(bis);
        }

        bis.close();
        fis.close();

        return cert;
    }

    // Returns the public key for a given Certificate.
    public static PublicKey getRSAPublicKey(Certificate cert)
    {
        return cert.getPublicKey();
    }

    // Takes a file path, returns the private key found there.  Throws exception if it cannot.
    public static PrivateKey getRSAPrivateKey(String filename) throws Exception
    {
        File file = new File(filename);
        FileInputStream fis = new FileInputStream(file);
        DataInputStream dis = new DataInputStream(fis);
        byte[] keyBytes = new byte[(int) file.length()];
        dis.readFully(keyBytes);
        dis.close();

        PKCS8EncodedKeySpec specification = new PKCS8EncodedKeySpec(keyBytes);
        KeyFactory factory = KeyFactory.getInstance("RSA");
        return factory.generatePrivate(specification);
    }

    // Exclusively exists to translate the prime number here: https://www.ietf.org/rfc/rfc3526.txt into something
    // useful.
    public static BigInteger getDHP()
    {
        String copyPaste = "FFFFFFFF FFFFFFFF C90FDAA2 2168C234 C4C6628B 80DC1CD1\n" +
                "      29024E08 8A67CC74 020BBEA6 3B139B22 514A0879 8E3404DD\n" +
                "      EF9519B3 CD3A431B 302B0A6D F25F1437 4FE1356D 6D51C245\n" +
                "      E485B576 625E7EC6 F44C42E9 A637ED6B 0BFF5CB6 F406B7ED\n" +
                "      EE386BFB 5A899FA5 AE9F2411 7C4B1FE6 49286651 ECE45B3D\n" +
                "      C2007CB8 A163BF05 98DA4836 1C55D39A 69163FA8 FD24CF5F\n" +
                "      83655D23 DCA3AD96 1C62F356 208552BB 9ED52907 7096966D\n" +
                "      670C354E 4ABC9804 F1746C08 CA18217C 32905E46 2E36CE3B\n" +
                "      E39E772C 180E8603 9B2783A2 EC07A28F B5C55DF0 6F4C52C9\n" +
                "      DE2BCBF6 95581718 3995497C EA956AE5 15D22618 98FA0510\n" +
                "      15728E5A 8AACAA68 FFFFFFFF FFFFFFFF";
        String noSpaces = copyPaste.replace(" ", "");
        String diffieHellmanPString = noSpaces.replace("\n", "");

        return new BigInteger(diffieHellmanPString, 16);
    }

    // Generates a DH Private Key based on the current time.
    public static BigInteger generateDHPrivateKey()
    {
        Date currentDate = new Date();
        Random rng = new Random(currentDate.getTime());

        return new BigInteger(64, rng);
    }

    // Takes in the DH Private Key and performs the necessary, crazy math to produce the public key.
    public static BigInteger generateDHPublicKey(BigInteger exponent)
    {
        BigInteger diffieHellmanG = new BigInteger("2");

        return diffieHellmanG.modPow(exponent, getDHP());
    }

    // Takes in a DH Public Key, an RSA Private Key, and an RSA Public Key (for verification) and signs the DH Public
    // Key using the RSA Private Key.
    public static BigInteger signPublicKey(BigInteger dhPublicKey, PrivateKey privateRSAKey, PublicKey publicRSAKey) throws Exception
    {
        byte[] data = dhPublicKey.toByteArray();
        Signature sig = Signature.getInstance("SHA256withRSA");
        sig.initSign(privateRSAKey);
        sig.update(data);
        byte[] signedBytes = sig.sign();

        sig.initVerify(publicRSAKey);
        sig.update(data);

        if(sig.verify(signedBytes) == true)
        {
            return new BigInteger(signedBytes);
        }
        else
        {
            throw new Exception("Incorrect signature.");
        }
    }

    public static boolean verifyCASignature(Certificate signedCert, Certificate caCert) throws Exception
    {
        PublicKey caKey = getRSAPublicKey(caCert);
        boolean test = true;
        try {
            signedCert.verify(caKey);
        } catch (Exception e) {
            e.printStackTrace();
            test = false;
        }
        if (test)
        {
            return test;
        }
        else
        {
            throw new Exception("Unable to verify certificate.");
        }
    }

    // Takes in a DH Public Key, a signed DH Public Key, and an RSA Public Key, and verifies that the signature on
    // the signed DH Public Key is valid.
    public static boolean verifyLocalSignature(BigInteger incomingDHKey, BigInteger signedDHKey, PublicKey incomingRSAKey) throws Exception
    {
        Signature sig = Signature.getInstance("SHA256withRSA");
        sig.initVerify(incomingRSAKey);
        sig.update(incomingDHKey.toByteArray());

        boolean retVal = sig.verify(signedDHKey.toByteArray());
        if (!retVal)
        {
            throw new Exception("Unable to verify local signature.");
        }
        return retVal;
    }

    // Performs the necessary math on the other party's public DH Key and your private DH key to create a shared
    // secret.
    public static BigInteger generateSharedSecret(BigInteger otherPartyPublic, BigInteger localPrivate)
    {
        return (otherPartyPublic).modPow(localPrivate, getDHP());
    }

    // Performs the function as outlined by the pseudocode in the assignment.
    public static byte[] hdkfExpand(byte[] input, String tag) throws Exception
    {
        byte concatenate = 1;
        tag = tag + concatenate;
        byte[] tagBytes = tag.getBytes();
        SecretKeySpec key = new SecretKeySpec(input, "SHA256");
        Mac HMAC = Mac.getInstance("HmacSHA256");
        HMAC.init(key);

        byte[] hashedValue = HMAC.doFinal(tagBytes);

            return Arrays.copyOfRange(hashedValue, 0, 16);
    }

    // Takes the client nonce and the shared secret and produces secret keys for the various encryption methods.
    public static SecretKeys makeSecretKeys(byte[] clientNonce, BigInteger sharedDHSecret) throws Exception
    {
        byte[] data = sharedDHSecret.toByteArray();
        SecretKeySpec firstKey = new SecretKeySpec(clientNonce, "SHA256");
        Mac HMAC = Mac.getInstance("HmacSHA256");
        HMAC.init(firstKey);

        byte[] prk = HMAC.doFinal(data);

        SecretKeySpec serverEncrypt = new SecretKeySpec(hdkfExpand(prk, "server encrypt"), "AES");
        SecretKeySpec clientEncrypt = new SecretKeySpec(hdkfExpand(serverEncrypt.getEncoded(), "client encrypt"),
                "AES");
        SecretKeySpec serverMAC = new SecretKeySpec(hdkfExpand(clientEncrypt.getEncoded(), "server MAC"), "SHA256");
        SecretKeySpec clientMAC = new SecretKeySpec(hdkfExpand(serverMAC.getEncoded(), "client MAC"), "SHA256");
        IvParameterSpec serverIV = new IvParameterSpec(hdkfExpand(clientMAC.getEncoded(), "server IV"));
        IvParameterSpec clientIV = new IvParameterSpec(hdkfExpand(serverIV.getIV(), "client IV"));

        return new SecretKeys(serverEncrypt, clientEncrypt, serverMAC, clientMAC, serverIV, clientIV);
    }

    // Takes the current messages, concatenated together, along with the appropriate keys and the source of the
    // encryption and HMACs the first.
    public static byte[] handshakeHMAC(ByteArrayOutputStream messageConcatenation, SecretKeys providedKeys,
                                       String source) throws Exception
    {
        byte[] HMACConcatenation = messageConcatenation.toByteArray();
        Mac HMAC = Mac.getInstance("HmacSHA256");
        if (source.equals("server"))
        {
            HMAC.init(providedKeys.serverMAC);
        }
        else
        {
            HMAC.init(providedKeys.clientMAC);
        }

        return HMAC.doFinal(HMACConcatenation);
    }

    // Takes in a payload, the appropriate keys, and a source identification, performs an HMAC on it,
    // concatenates that HMAC to the payload, encrypts the whole thing, and returns the concatenated payload.
    public static byte[] encryptForSending(byte[] payload, SecretKeys providedKeys, String source) throws Exception
    {
        Mac HMAC = Mac.getInstance("HmacSHA256");
        if (source.equals("server"))
        {
            HMAC.init(providedKeys.serverMAC);
        }
        else
        {
            HMAC.init(providedKeys.clientMAC);
        }
        byte[] hmacResult = HMAC.doFinal(payload);

        ByteArrayOutputStream concatenation = new ByteArrayOutputStream();
        concatenation.write(payload);
        concatenation.write(hmacResult);
        byte[] messageToEncrypt = concatenation.toByteArray();

        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        if (source.equals("server"))
        {
            cipher.init(Cipher.ENCRYPT_MODE, providedKeys.serverEncrypt, providedKeys.serverIV);
        }
        else
        {
            cipher.init(Cipher.ENCRYPT_MODE, providedKeys.clientEncrypt, providedKeys.clientIV);
        }
        return cipher.doFinal(messageToEncrypt);
    }

    // Takes in an encrypted payload, decrypts it, separates the HMAC from the actual message, verifies the HMAC
    // against the payload, and then returns the plaintext payload.
    public static byte[] decryptForReceipt(byte[] encryptedPayload, SecretKeys providedKeys, String source) throws Exception {
        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        if (source.equals("server")) {
            cipher.init(Cipher.DECRYPT_MODE, providedKeys.serverEncrypt, providedKeys.serverIV);
        } else {
            cipher.init(Cipher.DECRYPT_MODE, providedKeys.clientEncrypt, providedKeys.clientIV);
        }

        byte[] unencryptedPayload = cipher.doFinal(encryptedPayload);

        int hashStart = unencryptedPayload.length - 32;

        byte[] file = Arrays.copyOfRange(unencryptedPayload, 0, hashStart);
        byte[] hash = Arrays.copyOfRange(unencryptedPayload, hashStart, unencryptedPayload.length);

        Mac HMAC = Mac.getInstance("HmacSHA256");
        if (source.equals("server")) {
            HMAC.init(providedKeys.serverMAC);
        } else {
            HMAC.init(providedKeys.clientMAC);
        }
        byte[] hmacResult = HMAC.doFinal(file);

        if (java.util.Arrays.equals(hash, hmacResult)) {
            System.out.println("Hash of received data checks out.");
            return file;
        } else {
            throw new Exception("Received file corrupted.");
        }
    }
}
