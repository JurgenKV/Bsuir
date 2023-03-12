package com.company;

import java.io.*;
import java.net.*;
import java.util.*;


public class UdpServer {
    private DatagramSocket serverSocket;
    private int clientPort;
    private InetAddress clientIP;

    public UdpServer() {
        try {
            Scanner scan = new Scanner(System.in);

            System.out.print("Enter port number: ");
            int socketPort = scan.nextInt();
            serverSocket = new DatagramSocket(socketPort);

            System.out.println("Server started...");
        } catch (SocketException e) {
            System.err.println("Socket exception!");
            e.printStackTrace();
        }
    }

    private void getCommand() {

        while (true) {
            try {
                byte[] recvData = new byte[1024];
                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
                serverSocket.receive(recvPacket);

                clientIP = recvPacket.getAddress();
                clientPort = recvPacket.getPort();

                String command = new String(recvPacket.getData()).trim();
                processCommand(command);
            } catch (IOException ignored) {
            }
        }

    }

    private void processCommand(String command) {
        try {
            String[] words = command.split("\\s");
            switch (words[0]) {
                case "download" -> {
                    DownloadThread downloadThread = new DownloadThread(clientPort, clientIP, serverSocket, words);
                    downloadThread.start();
                    downloadThread.join();
                }
                case "upload" -> {
                    UploadThread uploadThread = new UploadThread(clientPort, clientIP, serverSocket, words);
                    uploadThread.start();
                    uploadThread.join();
                }
                case "time" -> {
                    TimeThread timeThread = new TimeThread(clientPort, clientIP, serverSocket);
                    timeThread.start();
                }
                case "echo" -> {
                    EchoThread echoThread = new EchoThread(clientPort, clientIP, serverSocket, words);
                    echoThread.start();
                }
                default -> {
                    String corrupt = "Command not recognized, possibly corrupted during transmission";
                    byte[] sendData = corrupt.getBytes();
                    DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
                    serverSocket.send(sendPacket);
                }
            }
        } catch (IOException | InterruptedException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        UdpServer server = new UdpServer();
        server.getCommand();
    }
}
