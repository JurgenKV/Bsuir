package com.company;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class TimeThread extends Thread{
    private final int clientPort;
    private final InetAddress clientIP;
    private final DatagramSocket serverSocket;

    public TimeThread(int clientPort, InetAddress clientIP, DatagramSocket serverSocket) {
        this.clientPort = clientPort;
        this.clientIP = clientIP;
        this.serverSocket = serverSocket;
    }

    @Override
    public void run() {
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();
        System.out.println("Execute time: " + dateFormat.format(date));
        sendString(dateFormat.format(date));
    }

    private void sendString(String message) {
        try {
            byte[] sendData = message.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
            serverSocket.send(sendPacket);
        } catch (IOException ex) {
            System.out.println("Error while sending message!");
            ex.printStackTrace();
        }
    }
}
