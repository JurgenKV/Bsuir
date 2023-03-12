package com.company;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.*;
import java.text.DecimalFormat;

public class DownloadThread extends Thread {
    private final int clientPort;
    private final InetAddress clientIP;
    private final DatagramSocket serverSocket;
    private final String[] message;
    private static final String SERVER_HOME = "D:\\dir\\server";

    public DownloadThread(int clientPort, InetAddress clientIP, DatagramSocket serverSocket, String[] message) {
        this.clientPort = clientPort;
        this.clientIP = clientIP;
        this.serverSocket = serverSocket;
        this.message = message;
    }

    @Override
    public void run() {
        System.out.println("Execute download");
        transmitFile(message[1]);
    }

    private void transmitFile(String file) {
        try {
            long start = System.nanoTime();

            byte[] recvData = new byte[10240];
            byte[] sendData = new byte[10240];

            String filePath = SERVER_HOME + "\\" + file;

            BufferedInputStream bis = new BufferedInputStream(new FileInputStream(filePath));
            serverSocket.setSoTimeout(100000);

            int packetsSent = 0;

            while (bis.read(sendData) != -1) {

                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
                serverSocket.send(sendPacket);
                packetsSent++;

                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);

                int rcvCheck = 0;
                while (rcvCheck == 0) {
                    try {
                        rcvCheck = 1;
                        serverSocket.receive(recvPacket);
                    } catch (SocketTimeoutException se) {
                        rcvCheck = 0;
                        System.out.println("Client return packet timed out");
                        System.out.println("Retransmitting...");
                        serverSocket.send(sendPacket);
                        packetsSent++;
                    }
                }
                recvData = new byte[10240];
                sendData = new byte[10240];
            }

            String end = "end";
            sendData = end.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
            serverSocket.send(sendPacket);

            long endTime = System.nanoTime();
            long difference = endTime - start;
            DecimalFormat decimalFormat = new DecimalFormat("#.0#");
            System.out.println("Bitrate: " + decimalFormat.format(packetsSent * 10240 / (difference / 1000000000.0)));
            System.out.println("Awaiting next command from client...");
            bis.close();
        } catch (FileNotFoundException ex) {
            System.err.println("Filename is incorrect!");
            ex.printStackTrace();
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }
}
