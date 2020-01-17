package sample.ui;


import com.sun.javafx.logging.Logger;
import javafx.application.Platform;

public class JavafxThread extends Thread {
    private static String actuallyChooseId = " ";
    public void run(){
        while(true) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            Controller.getInstance().refreshImage(actuallyChooseId);
        }
    }

    public static String getActuallyChooseId() {
        return actuallyChooseId;
    }

    public static void setActuallyChooseId(String actuallyChooseId) {
        JavafxThread.actuallyChooseId = actuallyChooseId;
    }

}
