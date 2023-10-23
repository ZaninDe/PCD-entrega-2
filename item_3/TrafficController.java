public class TrafficController {

    private boolean bridgeIsOccupied = false;

    private synchronized void leaveBrige() {
        bridgeIsOccupied = false;
        notifyAll();
    }

    private synchronized void enter() {
        while (bridgeIsOccupied) {
            try {
                wait();
            } catch(InterruptedException err) {
                Thread.currentThread().interrupt();
            }
        }
        bridgeIsOccupied = true;
        notifyAll();
    }


    public void enterLeft() {
        enter();
    }

    public void enterRight() {
        enter();
    }

    public void leaveLeft() {
        leaveBrige();
    }

    public void leaveRight() {
        leaveBrige();
    }

}