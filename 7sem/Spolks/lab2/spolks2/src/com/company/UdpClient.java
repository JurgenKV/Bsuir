package com.company;

import java.io.*;
import java.net.*;
import java.text.DecimalFormat;
import java.util.*;

public class UdpClient {
    InetAddress serverAddress;
    int serverPort;
    DatagramSocket clientSocket;
    String clientFolder;
    int lossRate;
    Random rand;
    Scanner scan;

    public UdpClient() {
        try {
            scan = new Scanner(System.in);

            System.out.print("Enter IP address:\n>");
            String ip = scan.nextLine();

            System.out.print("Enter port number:\n>");
            serverPort = scan.nextInt();

            serverAddress = InetAddress.getByName(ip);
            clientSocket = new DatagramSocket();

            clientFolder = "D:\\dir\\client";
            rand = new Random();
            scan.nextLine();
            welcomeUser();
        } catch (UnknownHostException e) {
            System.err.println("Incorrect IP!");
            e.printStackTrace();
        } catch (SocketException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void welcomeUser() {
        System.out.print("\n\n\n\n\n");
        System.out.println("Welcome to the client side!");
        System.out.println("\nPlease insert a command(echo, time, download, upload, list, exit)");
    }

    private void commandMode() {
        try {
            boolean leave = false;
            while (!leave) {
                System.out.print(">");

                String commandString = scan.nextLine();

                byte[] sendData;
                byte[] recvData = new byte[1024];

                DatagramPacket sendPacket;
                DatagramPacket recvPacket;

                if (commandString.equals("download") || commandString.equals("list") || commandString.equals("upload") || commandString.equals("time") || commandString.equals("echo")) {

                    sendData = commandString.getBytes();
                    sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                    clientSocket.send(sendPacket);

                    if (commandString.equals("download")) {
                        System.out.print("Enter file name to download:\n>");

                        commandString = scan.nextLine();

                        sendData = new byte[1024];

                        System.out.print("Preparing to receive file.\nSpecify loss probability (0-100):\n>");
                        lossRate = Integer.parseInt(scan.nextLine());

                        sendData = commandString.getBytes();
                        sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                        clientSocket.send(sendPacket);

                        receiveMode(commandString);
                    } else if (commandString.equals("time")) {
                        receiveString();
                    } else if (commandString.equals("echo")) {
                        System.out.print("Enter message:\n>");
                        commandString = scan.nextLine();

                        sendData = new byte[1024];

                        sendData = commandString.getBytes();
                        sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                        clientSocket.send(sendPacket);

                        receiveString();
                    } else if (commandString.equals("upload")) {
                        System.out.print("Enter file name to upload:\n>");
                        commandString = scan.nextLine();

                        sendData = commandString.getBytes();
                        sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                        clientSocket.send(sendPacket);

                        recvPacket = new DatagramPacket(recvData, recvData.length);
                        clientSocket.receive(recvPacket);

                        String file = new String(recvPacket.getData()).trim();
                        transmitFile(file);
                    } else {
                        recvPacket = new DatagramPacket(recvData, recvData.length);
                        clientSocket.receive(recvPacket);

                        String response = new String(recvPacket.getData()).trim();
                        System.out.println(response);
                    }
                } else if (commandString.equals("exit")) {
                    leave = true;
                } else {
                    System.out.println("Command not recognized.\nInput a new command(echo, time, download, upload, list, exit)");
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
            DatagramPacket recvPacket;

            recvPacket = new DatagramPacket(recvData, recvData.length);
            clientSocket.receive(recvPacket);
            String message = new String(recvPacket.getData()).trim();

            System.out.println("\n" + message);
            System.out.println("Input a new command(echo, time, download, upload, list, exit)");
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void receiveMode(String file) {
        try {
            FileOutputStream fos = new FileOutputStream(clientFolder + "\\" + file, true);

            int packetsIgnored = 0;
            int packetsWritten = 0;

            boolean receiving = true;
            while (receiving) {
                byte[] sendData = new byte[1024];
                byte[] recvData = new byte[1024];

                DatagramPacket recvPacket;
                DatagramPacket ackPacket;

                recvPacket = new DatagramPacket(recvData, recvData.length);
                clientSocket.receive(recvPacket);

                String endFile = new String(recvPacket.getData()).trim();

                if (endFile.equals("end")) {
                    System.out.println("File downloaded successfully!\n");
                    System.out.println("Packets ignored: " + packetsIgnored);
                    System.out.println("Packets written: " + packetsWritten);
                    System.out.println("\nInput a new command(echo, time, download, upload, list, exit)");
                    receiving = false;
                } else if (rand.nextInt(100) < lossRate) {
                    packetsIgnored++;
                    System.out.println("* LOST PACKET *");
                } else {
                    packetsWritten++;
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
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void transmitFile(String file) {
        try {
            long start = System.nanoTime();

            byte[] recvData = new byte[1024];
            byte[] sendData = new byte[1024];

            String filePath = clientFolder + "\\" + file;

            BufferedInputStream bis = new BufferedInputStream(new FileInputStream(filePath));
            clientSocket.setSoTimeout(5000);

            int packetsSent = 0;
            int check;
            while ((check = bis.read(sendData)) != -1) {

                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
                clientSocket.send(sendPacket);
                packetsSent++;

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
                        packetsSent++;
                    }
                }

                recvData = new byte[1024];
                sendData = new byte[1024];
            }

            String end = "end";
            sendData = end.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);
            clientSocket.send(sendPacket);
            packetsSent++;
            System.out.println("File sent successfully!\n");

            long endTime = System.nanoTime();
            long difference = endTime - start;
            DecimalFormat decimalFormat = new DecimalFormat("#.0#");
            System.out.println("Bitrate (b/s): " + decimalFormat.format((packetsSent - 1) * 1024 / (difference / 1000000000.0)));

            System.out.println("\nInput a new command(echo, time, download, upload, list, exit)");
            bis.close();
        } catch (FileNotFoundException ex) {
            System.err.println("Filename is incorrect!");
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void logout() {
        System.out.println("logging out");
        clientSocket.close();
    }

    public static void main(String[] args) {
        UdpClient client = new UdpClient();

        client.commandMode();
    }

}
