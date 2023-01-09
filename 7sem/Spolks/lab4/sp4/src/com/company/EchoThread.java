package com.company;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Arrays;
import java.util.stream.Collectors;

public class EchoThread extends Thread {
    private final int clientPort;
    private final InetAddress clientIP;
    private final DatagramSocket serverSocket;
    private final String[] message;

    public EchoThread(int clientPort, InetAddress clientIP, DatagramSocket serverSocket, String[] message) {
        this.clientPort = clientPort;
        this.clientIP = clientIP;
        this.serverSocket = serverSocket;
        this.message = message;
    }

    @Override
    public void run() {
        String msg = Arrays.stream(message).skip(1).collect(Collectors.joining(" "));
        System.out.println("Execute echo: " + msg);
        sendString(msg);
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
