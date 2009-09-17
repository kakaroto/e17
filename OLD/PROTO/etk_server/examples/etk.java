import java.io.*;

public class etk {

  private static String FIFO = "/tmp/etk_server_fifo";

  private static String sendCommand(String command) throws IOException {
    /* print null terminated string */
    FileOutputStream out = new FileOutputStream(FIFO);
    out.write(command.getBytes());
    out.write('\0');
    out.close();

    /* read null terminated string (-1's may follow the termination too */
    FileInputStream in = new FileInputStream(FIFO);
    int inChar;
    StringBuffer ret = new StringBuffer();
    while ((inChar = in.read()) > 0)
	ret.append((char) inChar);
    in.close();
    return ret.toString();
  }

  public static void main(String args[]) throws IOException {
     
    System.out.println("Deprecated for now!");
    return;
    
    Runtime.getRuntime().exec("etk_server " + FIFO);
    System.out.println("running");
    try {
      Thread.sleep(1000);
    } catch (InterruptedException e) {
    }

    sendCommand("etk_init");
    String window = sendCommand("etk_window_new");
    String button = sendCommand("etk_button_new_with_label \"Java Rules\"");
    String vbox = sendCommand("etk_vbox_new 0 0");
    sendCommand("etk_box_pack_start " + vbox + " " + button + " 1 1 0");
    sendCommand("etk_container_add " + window + " " + vbox);
    sendCommand("etk_widget_show_all " + window);

    sendCommand("etk_server_signal_connect \"clicked\" " + button + " \"button_1_click\"");
    sendCommand("etk_server_signal_connect \"delete_event\" " + window + " \"win_delete\"");

    String event = "";
    while (!event.equals("win_delete")) {
      event = sendCommand("etk_server_callback");

      if (event.equals("button_1_click"))
        System.out.println("Button 1 clicked!");
    }

    sendCommand("etk_main_quit");
    sendCommand("etk_server_shutdown");
  }

}
