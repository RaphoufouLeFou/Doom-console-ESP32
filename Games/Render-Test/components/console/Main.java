
import javax.swing.*;

import java.awt.BorderLayout;
import java.awt.Desktop;
import java.awt.FlowLayout;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.IOException;
import java.awt.event.ActionEvent;





public class Main {
    static File file = null;
    static int speed = 921600;
    static String port = "COM5";
    static int freq = 80;
    public static void main(String[] args){
        
        CreateFrame("test");
    }

    public static void CreateFrame(String title){

        Integer[] SpeedsINT = { 921600, 115200, 256000, 230400, 512000 };
        String[] Ports = { "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "COM10", "COM11", "COM12", "COM13", "COM14", "COM15", "COM16", "COM17", "COM18", "COM19", "COM20", };
        String[] Freqs = { "80MHz", "40MHz" };
        Integer[] FreqsINT = { 80, 40 };

        JComboBox<Integer> SpeedList = new JComboBox<>(SpeedsINT);
        SpeedList.setSelectedIndex(1);
        SpeedList.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                JComboBox cb = (JComboBox)e.getSource();
                int Speed = (int)cb.getSelectedItem();
                speed = Speed;
            }
        });

        JComboBox PortList = new JComboBox(Ports);
        PortList.setSelectedIndex(1);
        PortList.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                JComboBox cb = (JComboBox)e.getSource();
                String Port = (String)cb.getSelectedItem();
                port = Port;
            }
        });

        JComboBox FreqList = new JComboBox(Freqs);
        FreqList.setSelectedIndex(1);
        FreqList.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                JComboBox cb = (JComboBox)e.getSource();
                freq = FreqsINT[cb.getSelectedIndex()];
            }
        });
        
        JFrame  frame = new JFrame(title);
        JPanel panel = new JPanel(new BorderLayout());
        //panel.setLayout(new FlowLayout());
        JToolBar toolBar = new JToolBar();
        JButton buttonFile = new JButton();
        buttonFile.setText("File");
        buttonFile.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    openDirectory();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
        });
        toolBar.add(buttonFile);
        toolBar.add( new JCheckBox( "Check me" ) );
        JButton button = new JButton();
        button.setText("Upload");
        button.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Upload(speed, port, freq);
            }
        });


        panel.add(toolBar, BorderLayout.NORTH);
        panel.add(SpeedList, BorderLayout.WEST);
        panel.add(PortList, BorderLayout.CENTER);
        panel.add(FreqList, BorderLayout.EAST);
        panel.add(button, BorderLayout.PAGE_END);
        frame.add(panel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(800, 600);
        frame.setVisible(true);
    }

    public static void Upload(int Speed, String Port, int freq){

        if(file == null){
            System.out.println("No file provided!\n");
            return;
        }

        String cmd = "C:\\Espressif\\python_env\\idf4.1_py3.8_env\\Scripts\\python.exe C:\\Users\\rapha\\esp\\esp-idf\\components\\esptool_py\\esptool\\esptool.py -p " + Port + " -b " + Speed + " --before default_reset --after hard_reset --chip esp32 write_flash --flash_mode dio --flash_freq " + freq + "m --flash_size detect 0x100000 " + file ;
        System.out.println(cmd);
        try {
            Process process = Runtime.getRuntime().exec(cmd);
            BufferedReader stdInput = new BufferedReader(new InputStreamReader(process.getInputStream()));
            BufferedReader stdError = new BufferedReader(new InputStreamReader(process.getErrorStream()));

            String s = null;
            while ((s = stdInput.readLine()) != null) {
                System.out.println(s);
            }
            
            if((s = stdError.readLine()) != null) {
                SendError();
            }
        } catch(Exception ex){
            ex.printStackTrace();
        }
    }

    public static void SendError(){
        System.out.println("An error occured\n");
    }

    public static void openDirectory() throws IOException {

        JFileChooser chooser = new JFileChooser();
        int result = chooser.showOpenDialog(null);
        if(result == JFileChooser.APPROVE_OPTION){
            file = new File(chooser.getSelectedFile().getAbsolutePath());
            System.out.println(file);
        }
      }
}

