/*
App is OI Safe 1.4.6;  org.openintents.safe.apk
javac -cp bcprov-jdk15on-1.70.jar OISafeDecryptor.java && java -cp .:bcprov-jdk15on-1.70.jar OISafeDecryptor TestTest b2e7e20476b362f6 ac911ad4d87d6ce56bbad16c3631792c08f07c497ea2ddd15f471d64672f6ab4c088b10571ea89f73274abeb8a21575fec57bc7295e86bce730ef0265a1446cc48fd82089ef2e8bb746f877e7367f59f 5c433a6aef892c34446aec1cedf85f14376ecba2a9fcd04de66a5dd146e17b68 
*/


import org.bouncycastle.jce.provider.BouncyCastleProvider;
import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.PBEParameterSpec;
import java.nio.charset.StandardCharsets;
import java.security.Security;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class OISafeDecryptor {

    private static final String ALG_MEDIUM = "PBEWithMD5And128BitAES-CBC-OpenSSL";
    private static final String ALG_STRONG = "PBEWithSHA1And256BitAES-CBC-BC";
    private static final int[] ITER_CANDIDATES_MASTER = {20, 1};
    private static final String[] ALG_CANDIDATES_MASTER = {ALG_MEDIUM, ALG_STRONG};
    private static final int[] ITER_CANDIDATES_FIELDS = {1};
    private static final String[] ALG_CANDIDATES_FIELDS = {ALG_MEDIUM};

    static { Security.addProvider(new BouncyCastleProvider()); }

    public static void main(String[] args) throws Exception {
        if (args.length < 3) {
            System.err.println("Usage: OISafeDecryptor <password> <saltHex> <masterHex> [<fieldHex1> ...]");
            System.exit(1);
        }

        String password = args[0];
        byte[] salt = hexToBytes(args[1]);
        byte[] encryptedMaster = hexToBytes(args[2]);
        List<String> fields = args.length > 3 ? Arrays.asList(args).subList(3, args.length) : new ArrayList<>();

        // Decrypt master key
        byte[] masterKey = null;
        for (String alg : ALG_CANDIDATES_MASTER) {
            for (int iter : ITER_CANDIDATES_MASTER) {
                try {
                    masterKey = decryptPBE(alg, iter, password, salt, encryptedMaster);
                    break;
                } catch (Exception ignored) {}
            }
            if (masterKey != null) break;
        }

        if (masterKey == null) {
            System.err.println("Failed to decrypt master key.");
            System.exit(1);
        }

        String masterKeyStr = new String(masterKey, StandardCharsets.UTF_8);
        System.out.println("Master: " + masterKeyStr);

        // Decrypt fields
        for (String fieldHex : fields) {
            byte[] encryptedField = hexToBytes(fieldHex);
            for (String alg : ALG_CANDIDATES_FIELDS) {
                for (int iter : ITER_CANDIDATES_FIELDS) {
                    try {
                        byte[] dec = decryptPBE(alg, iter, masterKeyStr, salt, encryptedField);
                        System.out.println("Field: " + new String(dec, StandardCharsets.UTF_8));
                    } catch (Exception ignored) {}
                }
            }
        }
    }

    private static byte[] decryptPBE(String alg, int iter, String pass, byte[] salt, byte[] data) throws Exception {
        SecretKey key = SecretKeyFactory.getInstance(alg, "BC").generateSecret(new PBEKeySpec(pass.toCharArray()));
        Cipher cipher = Cipher.getInstance(alg, "BC");
        cipher.init(Cipher.DECRYPT_MODE, key, new PBEParameterSpec(salt, iter));
        return cipher.doFinal(data);
    }

    private static byte[] hexToBytes(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + Character.digit(s.charAt(i + 1), 16));
        }
        return data;
    }

    private static String byteToHex(byte[] bytes) {
        StringBuilder sb = new StringBuilder(bytes.length * 2);
        for (byte b : bytes) sb.append(String.format("%02x", b));
        return sb.toString();
    }
}
