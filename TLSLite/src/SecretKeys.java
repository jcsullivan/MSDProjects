import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class SecretKeys {

    SecretKeySpec serverEncrypt;
    SecretKeySpec clientEncrypt;
    SecretKeySpec serverMAC;
    SecretKeySpec clientMAC;
    IvParameterSpec serverIV;
    IvParameterSpec clientIV;

    public SecretKeys(SecretKeySpec serverEncrypt, SecretKeySpec clientEncrypt, SecretKeySpec serverMAC,
                      SecretKeySpec clientMAC, IvParameterSpec serverIV, IvParameterSpec clientIV)
    {
        this.serverEncrypt = serverEncrypt;
        this.clientEncrypt = clientEncrypt;
        this.serverMAC = serverMAC;
        this.clientMAC = clientMAC;
        this.serverIV = serverIV;
        this.clientIV = clientIV;
    }
}
