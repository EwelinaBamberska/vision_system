package vision.ui;


public class JavafxThread extends Thread {
    private static String actuallyChooseId = " ";

    //run refreshing image with 100 ms frequency
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
