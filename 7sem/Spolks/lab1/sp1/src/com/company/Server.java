package com.company;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.DateFormat;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.stream.Collectors;

public class Server {
    private Socket socket;
    private ServerSocket server;
    private static final String WORK_DIRECTORY_PATH = "D:\\work_directory_for_server\\";
    private static final int SIZE = 4;
    private String lastClient = "";
    private String lastCommand = "";
    private String lastFileName;
    private int leftFileLength;
    private static final int BYTES_FOR_READ_WRITE = 1024;
    private FileOutputStream outputStream;

    private byte[] readFile(String path) throws IOException {
        return Files.readAllBytes(Paths.get(path));
    }

    private int readLength(DataInputStream socketReader) throws IOException {
        byte[] resultArray = new byte[SIZE];
        int read = socketReader.read(resultArray, 0, SIZE);
        if (read <= -1) {
            System.err.println("Can't read data from socket");
        }
        return ByteBuffer.wrap(resultArray).getInt();
    }

    private String readMessage(DataInputStream socketReader) throws IOException {
        int commandLength = readLength(socketReader);

        byte[] bts = new byte[commandLength];
        int res = socketReader.read(bts, 0, commandLength);
        if (res < 0) {
            throw new IOException("Cant' read data from socket");
        }
        return new String(bts);
    }

    private void writeFile(DataOutputStream socketWriter, DataInputStream socketReader, byte[] file) throws IOException, InterruptedException {
        int fileLength = file.length;
        int tempLength = file.length;
        int size = Math.min(fileLength, BYTES_FOR_READ_WRITE);
        int from = 0;
        int to = from + size;
        long start = System.nanoTime();
        while (fileLength != 0) {
            byte[] temp = Arrays.copyOfRange(file, from, to);
            socketWriter.write(temp);
            socketWriter.flush();

            try {
                if (!socket.isInputShutdown() || !socket.isOutputShutdown()) {

                    byte ack = socketReader.readByte();
                    if (ack != 6) {
                        System.out.println("Wait 6 ack");
                        continue;
                    }
                } else {
                    System.err.println("Problem");
                    throw new InterruptedException();
                }
            } catch (SocketTimeoutException e) {
                runServer(5003);
            }

            from += size;
            fileLength -= size;
            if (fileLength < size) {
                size = fileLength;
            }
            to += size;
            leftFileLength = fileLength;
        }

        long end = System.nanoTime();
        long difference = end - start;
        DecimalFormat decimalFormat = new DecimalFormat("#.0#");
        System.out.println("Bitrate (b/s): " + decimalFormat.format(tempLength / (difference / 1000000000.0)));
    }

    private void writeInSocket(DataOutputStream socketWriter, String message) throws IOException {
        int commandLength = message.length();
        writeLength(socketWriter, commandLength);
        socketWriter.write(message.getBytes());
        socketWriter.flush();
    }

    private void saveFile(DataInputStream socketReader, DataOutputStream socketWriter, String fileName, int fileLength)
            throws IOException {
        int size = Math.min(fileLength, BYTES_FOR_READ_WRITE);
        int tempLength = fileLength;
        outputStream = new FileOutputStream(fileName, true);
        long start = System.nanoTime();
        while (fileLength != 0) {
            if (fileLength < size) {
                size = fileLength;
            }
            byte[] partOfFile = new byte[size];
            try {
                int status = socketReader.read(partOfFile, 0, size);
                if (status < 0) {
                    System.err.println("Problem with reading bytes");
                    throw new IOException();
                }
            } catch (SocketTimeoutException e) {
                outputStream.close();
                runServer(5003);
            }
            outputStream.write(partOfFile);
            fileLength -= size;
            leftFileLength = fileLength;
            socketWriter.writeByte(6);
            socketWriter.flush();
        }

        long end = System.nanoTime();
        long difference = end - start;
        DecimalFormat decimalFormat = new DecimalFormat("#.0#");
        System.out.println("Bitrate (b/s): " + decimalFormat.format(tempLength / (difference / 1000000000.0)));
        outputStream.close();
    }

    private void closeConnection(Socket socket, DataInputStream socketRead,
                                 DataOutputStream socketWrite) throws IOException {
        socket.close();
        socketRead.close();
        socketWrite.close();
    }

    private void writeLength(DataOutputStream socketWriter, int fileLength) throws IOException {
        byte[] bytes = ByteBuffer.allocate(SIZE).putInt(fileLength).array();
        socketWriter.write(bytes);
        socketWriter.flush();
    }

    private String executeEcho(String[] request) {
        return Arrays.stream(request).skip(1).collect(Collectors.joining(" "));
    }

    private String executeTime() {
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();
        return dateFormat.format(date);
    }

    private void executeDownloadCommand(DataOutputStream socketWriter,
                                        DataInputStream socketReader, String fileName)
            throws IOException, InterruptedException {
        if (!new File(WORK_DIRECTORY_PATH + fileName).exists()) {
            String error = String.format("File with name %s not found", fileName);
            System.err.print(error);
            writeLength(socketWriter, 0);
            writeInSocket(socketWriter, error);
        } else {
            byte[] file = readFile(WORK_DIRECTORY_PATH + fileName);
            writeLength(socketWriter, file.length);
            writeFile(socketWriter, socketReader, file);
            writeInSocket(socketWriter, "File successfully delivered to client");
            leftFileLength = 0;
            lastFileName = "";
        }
    }

    private void executeUploadCommand(DataInputStream socketReader, DataOutputStream socketWriter,
                                      String fileName) throws IOException {
        int fileLength = readLength(socketReader);
        if (fileLength != 0) {
            saveFile(socketReader, socketWriter, WORK_DIRECTORY_PATH + fileName, fileLength);
            writeInSocket(socketWriter,
                    "File " + fileName + " successfully received and saved");
        }
    }

    public void runServer(int port) {
        while (true) {
            try {
                server = new ServerSocket(port);
                server.setSoTimeout(100000);
                System.out.println("Server started");
                System.out.println("Waiting for a client ...");

                socket = server.accept();

                System.out.println("Client accepted");
                DataInputStream socketReader = new DataInputStream(
                        new BufferedInputStream(socket.getInputStream()));
                DataOutputStream socketWriter = new DataOutputStream(socket.getOutputStream());
                writeInSocket(socketWriter, "Connection successful");

                String clientAddress = readMessage(socketReader);
                System.out.println("Client address: " + clientAddress);

                writeInSocket(socketWriter, lastClient.equals(clientAddress) ? lastCommand : "");
                if (lastClient.equals(clientAddress)) {
                    System.out.printf("%s had time to reconnect", clientAddress);
                    if (lastCommand.equals("upload")) {
                        writeInSocket(socketWriter, lastFileName);
                        writeLength(socketWriter, leftFileLength);
                        saveFile(socketReader, socketWriter, WORK_DIRECTORY_PATH + lastFileName,
                                leftFileLength);
                        writeInSocket(socketWriter,
                                "File " + lastFileName + " successfully received and saved");
                        lastClient = null;
                        lastCommand = "";
                        leftFileLength = 0;
                    }
                    if (lastCommand.equals("download")) {
                        byte[] file = readFile(WORK_DIRECTORY_PATH + lastFileName);
                        writeInSocket(socketWriter, lastFileName);
                        int fileLength = readLength(socketReader);
                        int realLeftFileLength = file.length - fileLength;
                        writeLength(socketWriter, realLeftFileLength);
                        if (fileLength != 0) {
                            writeFile(socketWriter, socketReader, Arrays.copyOfRange(file, fileLength, file.length));
                            writeInSocket(socketWriter, "Command download finished");

                        }
                        lastClient = null;
                        lastCommand = "";
                        leftFileLength = 0;

                    }
                }
                lastClient = clientAddress;

                String request = "";
                String command = "";
                while (!command.equals("exit")) {
                    request = readMessage(socketReader);
                    System.out.println(request);
                    String[] words = request.split("\\s");
                    command = words[0].toLowerCase();
                    lastCommand = command;
                    if (!command.isEmpty()) {
                        switch (command) {
                            case "echo":
                                writeInSocket(socketWriter, executeEcho(words));
                                break;
                            case "time":
                                writeInSocket(socketWriter, executeTime());
                                break;
                            case "exit":
                                writeInSocket(socketWriter, "Server start closing connection");
                                break;
                            case "upload":
                                lastFileName = words[1];
                                executeUploadCommand(socketReader, socketWriter, words[1]);
                                lastFileName = "";
                                leftFileLength = 0;
                                break;
                            case "download":
                                lastFileName = words[1];
                                executeDownloadCommand(socketWriter, socketReader, words[1]);
                                lastFileName = "";
                                leftFileLength = 0;
                                break;
                            default:
                                writeInSocket(socketWriter, "Command not found");
                                Thread.sleep(100);
                                break;
                        }
                    }
                }
                System.out.println("Closing connection");
                closeConnection(socket, socketReader, socketWriter);
            } catch (SocketTimeoutException e) {
                System.err.println("Server is tired of waiting client connection");
                try {
                    if (socket != null) {
                        socket.close();
                    }
                    if (server != null) {
                        server.close();
                    }
                    if (outputStream != null) {
                        outputStream.close();
                    }
                    lastCommand = "";
                    lastClient = "";
                    leftFileLength = 0;
                    lastFileName = "";
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            } catch (IOException | InterruptedException i) {
                System.err.println("Connection issue");
                try {
                    if (socket != null) {
                        socket.close();
                    }
                    if (server != null) {
                        server.close();
                    }
                    if (outputStream != null) {
                        outputStream.close();
                    }
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        }
    }

    public static void main(String[] args) {
        new Server().runServer(5003);
    }

}
