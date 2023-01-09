package com.company;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.text.DecimalFormat;

public class UploadThread extends Thread {
    private final int clientPort;
    private final InetAddress clientIP;
    private final DatagramSocket serverSocket;
    private final String[] message;
    private static final String SERVER_HOME = "D:\\dir\\server";

    public UploadThread(int clientPort, InetAddress clientIP, DatagramSocket serverSocket, String[] message) {
        this.clientPort = clientPort;
        this.clientIP = clientIP;
        this.serverSocket = serverSocket;
        this.message = message;
    }

    @Override
    public void run() {
        try {
            System.out.println("Execute upload");
            String filename = message[1];

            byte[] sendData = new byte[10240];

            sendData = filename.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
            serverSocket.send(sendPacket);

            receiveMode(filename);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void receiveMode(String file) {
        try {
            long start = System.nanoTime();
            FileOutputStream fos = new FileOutputStream(SERVER_HOME + "\\" + file, true);

            int packetsWritten = 0;

            boolean receiving = true;
            while (receiving) {
                byte[] sendData = new byte[10240];
                byte[] recvData = new byte[10240];

                DatagramPacket ackPacket;
                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);

                //while(true) {
                //    try {
                        serverSocket.receive(recvPacket);
                //        break;
                //    } catch (IOException ignored) {

                //    }
                //}
                String endFile = new String(recvPacket.getData()).trim();

                if (endFile.equals("end")) {
                    System.out.println("File transfer complete");
                    receiving = false;
                } else {
                    packetsWritten++;
                    fos.write(recvPacket.getData());

                    String ack = "ack";
                    sendData = ack.getBytes();
                    ackPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
                    serverSocket.send(ackPacket);
                }
            }

            long endTime = System.nanoTime();
            long difference = endTime - start;
            DecimalFormat decimalFormat = new DecimalFormat("#.0#");
            System.out.println("Bitrate: " + decimalFormat.format(packetsWritten * 10240 / (difference / 1000000000.0)));
            System.out.println("Awaiting next command from client...");
            fos.close();
        } catch (FileNotFoundException ex) {
            System.err.println("Filename is incorrect!");
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }
}
