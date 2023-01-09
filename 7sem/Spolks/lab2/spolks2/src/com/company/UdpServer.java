package com.company;

import java.io.*;
import java.net.*;
import java.text.DateFormat;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.*;


public class UdpServer {
    private DatagramSocket serverSocket;

    private int socketPort;
    private int clientPort;

    private static final String SERVER_HOME = "D:\\dir\\server";
    private static final int BUFFER_SIZE = 1024;
    private String[] serverFiles;
    private InetAddress clientIP;


    public UdpServer() {
        try {
            Scanner scan = new Scanner(System.in);

            System.out.println("Enter port number:");
            int socketPort = scan.nextInt();

            serverSocket = new DatagramSocket(socketPort);
            serverFiles = getFiles(SERVER_HOME); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            System.out.println("Server started...");
        } catch (SocketException e) {
            System.err.println("Socket exception!");
            e.printStackTrace();
        }
    }

//    private void welcomeUser() {
//        System.out.println("Server started...");
//        //System.out.println("Press ctrl-c to exit.");
//    }

    private void getCommand() {
        try {
            while (true) {
                byte[] recvData = new byte[BUFFER_SIZE];

                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
                serverSocket.receive(recvPacket);

                clientIP = recvPacket.getAddress();
                clientPort = recvPacket.getPort();

                String clientCommand = new String(recvPacket.getData()).trim();

                processCommand(clientCommand);
            }
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void processCommand(String command) {
        try {
            byte[] recvData = new byte[1024];
            byte[] sendData = new byte[1024];

            if (command.equals("download")) {
                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
                serverSocket.receive(recvPacket);

                String file = new String(recvPacket.getData()).trim();

                transmitFile(file);
                serverSocket = new DatagramSocket(socketPort);
            } else if (command.equals("upload")) {
                recvData = new byte[1024];

                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
                serverSocket.receive(recvPacket);
                String filename = new String(recvPacket.getData()).trim();

                sendData = new byte[1024];
                System.out.print("Preparing to uploading file.\n");

                sendData = filename.getBytes();
                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
                serverSocket.send(sendPacket);

                receiveMode(filename);
            } else if (command.equals("time")) {
                sendTime();
            } else if (command.equals("echo")) {
                recvData = new byte[1024];

                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
                serverSocket.receive(recvPacket);
                String message = new String(recvPacket.getData()).trim();
                System.out.println("Execute echo: " + message);
                sendString(message);
            } else if (command.equals("list")) {
                String files = filesToString(serverFiles);
                sendData = files.getBytes();
                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
                serverSocket.send(sendPacket);
            } else {
                String corrupt = "Command not recognized, possibly corrupted during transmission";
                sendData = corrupt.getBytes();
                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
                serverSocket.send(sendPacket);
            }
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private void sendTime() {
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();
        System.out.println("Execute time: " + dateFormat.format(date));
        sendString(dateFormat.format(date));
    }

    private void sendString(String message) {
        try {
            DatagramPacket sendPacket;
            byte[] sendData = message.getBytes();
            sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
            serverSocket.send(sendPacket);
        } catch (IOException ex) {
            System.out.println("Error while sending message!");
            ex.printStackTrace();
        }
    }

    private void transmitFile(String file) {
        try {
            long start = System.nanoTime();

            byte[] recvData = new byte[1024];
            byte[] sendData = new byte[1024];

            String filePath = SERVER_HOME + "\\" + file;

            BufferedInputStream bis = new BufferedInputStream(new FileInputStream(filePath));
            serverSocket.setSoTimeout(5000);

            int packetsSent = 0;
            int check;
            while ((check = bis.read(sendData)) != -1) {

                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
                serverSocket.send(sendPacket);
                packetsSent++;
                System.out.println("data sent");

                DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);

                int rcvCheck = 0;
                while (rcvCheck == 0) {
                    try {
                        rcvCheck = 1;
                        serverSocket.receive(recvPacket);
                    } catch (SocketTimeoutException se) {
                        rcvCheck = 0;
                        System.out.println("Client return packet timed out.\nRetransmitting...");
                        serverSocket.send(sendPacket);
                        packetsSent++;
                    }
                }
                recvData = new byte[1024];
                sendData = new byte[1024];
            }

            String end = "end";
            sendData = end.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
            serverSocket.send(sendPacket);
            packetsSent++;
            System.out.println("done sending data\n");
            System.out.println("Total packets sent: " + packetsSent);
            System.out.println("\nAwaiting next command from client.");

            long endTime = System.nanoTime();
            long difference = endTime - start;
            DecimalFormat decimalFormat = new DecimalFormat("#.0#");
            System.out.println("Bitrate (b/s): " + decimalFormat.format((packetsSent - 1) * 1024 / (difference / 1000000000.0)));

            bis.close();
            serverSocket.close();
        } catch (FileNotFoundException ex) {
            System.err.println("Filename is incorrect!");
        } catch (IOException e) {
            System.err.println("Connection terminated!");
            e.printStackTrace();
        }
    }

    private String[] getFiles(String directory) {
        File folderToRead = new File(directory);

        File[] folderFiles = folderToRead.listFiles();

        String[] fileNames = new String[folderFiles.length];
        int fileCount = 0;
        for (int i = 0; i < folderFiles.length; i++) {
            if (folderFiles[i].isFile()) {
                fileNames[fileCount] = folderFiles[i].getName();
                fileCount++;
            }
        }
        return fileNames;
    }

    private String filesToString(String[] files) {
        String filesToString = "";
        for (int i = 0; i < files.length; i++) {
            filesToString = filesToString + files[i] + "\n";
        }
        return filesToString;
    }

    private void receiveMode(String file) {
        try {
            FileOutputStream fos = new FileOutputStream(SERVER_HOME + "\\" + file, true);

            int packetsIgnored = 0;
            int packetsWritten = 0;

            boolean receiving = true;
            while (receiving) {
                byte[] sendData = new byte[1024];
                byte[] recvData = new byte[1024];

                DatagramPacket recvPacket;
                DatagramPacket ackPacket;

                recvPacket = new DatagramPacket(recvData, recvData.length);
                serverSocket.receive(recvPacket);
                String endFile = new String(recvPacket.getData()).trim();

                if (endFile.equals("end")) {
                    System.out.println("File transfer complete\n");
                    System.out.println("Packets ignored: " + packetsIgnored);
                    System.out.println("Packets written: " + packetsWritten);
                    System.out.println("\nAwaiting next command from client.");
                    receiving = false;
                } else {
                    packetsWritten++;
                    fos.write(recvPacket.getData());
                    System.out.println("data written");
                    String ack = "ack";
                    sendData = ack.getBytes();
                    ackPacket = new DatagramPacket(sendData, sendData.length, clientIP, clientPort);
                    serverSocket.send(ackPacket);
                    System.out.println("ack sent");
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

    public static void main(String[] args) {
        UdpServer server = new UdpServer();

        server.getCommand();
    }
}
