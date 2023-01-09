package com.company;

import java.io.*;
import java.net.*;
import java.util.*;

public class UdpClient {
    InetAddress serverAddress;
    int serverPort;
    DatagramSocket clientSocket;
    String clientFolder;
    Scanner scan;

    public UdpClient() {
        try {
            scan = new Scanner(System.in);

            System.out.print("Enter IP address: ");
            String ip = scan.nextLine();

            System.out.print("Enter port number: ");
            serverPort = scan.nextInt();

            serverAddress = InetAddress.getByName(ip);
            clientSocket = new DatagramSocket();

            clientFolder = "D:\\dir\\client";
            scan.nextLine();

            System.out.print("Please insert a command(echo, time, download, upload, exit): ");

        } catch (UnknownHostException e) {
            System.err.println("Incorrect IP!");
            e.printStackTrace();
        } catch (SocketException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void commandMode() {
        try {
            boolean leave = false;
            while (!leave) {
                String commandString = scan.nextLine();
                String[] words = commandString.split("\\s");

                byte[] sendData = new byte[1024];
                byte[] recvData = new byte[1024];

                DatagramPacket sendPacket;
                DatagramPacket recvPacket;

                if (words[0].equals("download") || words[0].equals("list") || words[0].equals("upload") || words[0].equals("time") || words[0].equals("echo")) {

                    sendData = commandString.getBytes();
                    sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                    clientSocket.send(sendPacket);

                    switch (words[0]) {
                        case "download" -> {
                            receiveMode(words[1]);
                        }
                        case "time", "echo" -> receiveString();
                        case "upload" -> transmitFile(words[1]);
                        default -> {
                            recvPacket = new DatagramPacket(recvData, recvData.length);
                            clientSocket.receive(recvPacket);
                            String response = new String(recvPacket.getData()).trim();
                            System.out.println(response);
                        }
                    }
                } else if (commandString.equals("exit")) {
                    leave = true;
                } else {
                    System.out.println("Command not recognized");
                    System.out.print("Input a new command(echo, time, download, upload, exit): ");
                }
            }
            logout();
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void receiveString() {
        try {
            byte[] recvData = new byte[1024];
            DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
            clientSocket.receive(recvPacket);
            System.out.println(new String(recvPacket.getData()).trim());
            System.out.print("Input a new command(echo, time, download, upload, exit): ");
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void receiveMode(String file) {
        try {
            FileOutputStream fos = new FileOutputStream(clientFolder + "\\" + file, true);

            boolean receiving = true;
            while (receiving) {
                byte[] sendData = new byte[1024];
                byte[] recvData = new byte[1024];

                DatagramPacket ackPacket;

                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
                clientSocket.receive(recvPacket);
                String endFile = new String(recvPacket.getData()).trim();

                if (endFile.equals("end")) {
                    System.out.println("File downloaded successfully!");
                    System.out.print("Input a new command(echo, time, download, upload, exit): ");
                    receiving = false;
                } else {
                    fos.write(recvPacket.getData());

                    String ack = "ack";
                    sendData = ack.getBytes();
                    ackPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                    clientSocket.send(ackPacket);
                }
            }
            fos.close();
        } catch (FileNotFoundException ex) {
            System.err.println("Filename is incorrect!");
            ex.printStackTrace();
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void transmitFile(String file) {
        try {
            byte[] recvData = new byte[1024];
            byte[] sendData = new byte[1024];

            String filePath = clientFolder + "\\" + file;

            BufferedInputStream bis = new BufferedInputStream(new FileInputStream(filePath));
            clientSocket.setSoTimeout(5000);

            while (bis.read(sendData) != -1) {

                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                clientSocket.send(sendPacket);

                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);

                int rcvCheck = 0;
                while (rcvCheck == 0) {
                    try {
                        rcvCheck = 1;
                        clientSocket.receive(recvPacket);
                    } catch (SocketTimeoutException se) {
                        rcvCheck = 0;
                        System.out.println("Retransmitting...");
                        clientSocket.send(sendPacket);
                    }
                }

                recvData = new byte[1024];
                sendData = new byte[1024];
            }

            String end = "end";
            sendData = end.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
            clientSocket.send(sendPacket);

            DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
            clientSocket.receive(recvPacket);
            String ack = new String(recvPacket.getData()).trim();
            System.out.println("Ack received: " + ack);
            System.out.println("File sent successfully!");

            System.out.print("Input a new command(echo, time, download, upload, exit): ");
            bis.close();
        } catch (FileNotFoundException ex) {
            System.err.println("Filename is incorrect!");
            ex.printStackTrace();
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void logout() {
        System.out.println("Logging out");
        clientSocket.close();
    }

    public static void main(String[] args) {
        UdpClient client = new UdpClient();
        client.commandMode();
    }
}
