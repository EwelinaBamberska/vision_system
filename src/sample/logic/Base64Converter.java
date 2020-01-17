package sample.logic;

import sun.misc.BASE64Decoder;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.*;

public class Base64Converter {

    private static BASE64Decoder decoder = new BASE64Decoder();

    public static void decodeBase64String(String clientId){

        BufferedImage image = null;
        byte[] imageByte = new byte[0];
        String imageString = null;
        StringBuilder builder = null;

        try {
            InputStream is = new FileInputStream("c" + clientId + ".txt");
            BufferedReader buf = new BufferedReader(new InputStreamReader(is));

            String line = buf.readLine();
            builder = new StringBuilder();
            while (line != null) {
                builder.append(line).append("\n");
                line = buf.readLine();
            }
            imageByte = decoder.decodeBuffer(String.valueOf(builder));
            ByteArrayInputStream bis = new ByteArrayInputStream(imageByte);
            image = ImageIO.read(bis);
            bis.close();

        // write the image to a file
            File outputfile = new File("i" + clientId + ".jpg");

            ImageIO.write(image, "jpg", outputfile);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
