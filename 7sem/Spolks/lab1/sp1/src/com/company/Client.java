package com.company;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.Scanner;

public class Client {

    private Scanner scanner = new Scanner(System.in);
    private static final String WORK_DIRECTORY_PATH = "D:\\work_directory_for_client\\";
    private static final String SERVER_ADDRESS = "192.168.100.5";
    private static final int SERVER_PORT = 5003;
    private static final int SIZE = 4;
    private static final int BYTE_FOR_READ_WRITE = 1024;
    private FileOutputStream outputStream;
    private Socket socket;
    private volatile int dataSize;
    private volatile boolean isNeedStop;

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

    private void writeFile(DataOutputStream socketWriter, DataInputStream socketReader, byte[] file)
            throws IOException, InterruptedException {
        int fileLength = file.length;
        int size = Math.min(fileLength, BYTE_FOR_READ_WRITE);
        int from = 0;
        int to = from + size;
        while (fileLength != 0) {
            byte[] temp = Arrays.copyOfRange(file, from, to);
            socketWriter.write(temp);
            socketWriter.flush();
            try {
                if (!socket.isInputShutdown() || !socket.isOutputShutdown()) {

                    byte ack = socketReader.readByte();
                    if (ack != 6) {
                        continue;
                    }
                } else {
                    throw new InterruptedException();
                }
            } catch (SocketTimeoutException e) {
                runClient(SERVER_ADDRESS, SERVER_PORT);
            }
            from += size;
            fileLength -= size;
            dataSize += size;
            if (fileLength < size) {
                size = fileLength;
            }
            to += size;
        }
        isNeedStop = true;
    }

    private void writeInSocket(DataOutputStream socketWriter, String message) throws IOException {
        int commandLength = message.length();
        writeLength(socketWriter, commandLength);
        socketWriter.write(message.getBytes());
        socketWriter.flush();
    }

    private void saveFile(DataInputStream socketReader, DataOutputStream socketWriter,
                          String fileName, int fileLength)
            throws IOException {
        int size = Math.min(fileLength, BYTE_FOR_READ_WRITE);
        outputStream = new FileOutputStream(fileName, true);
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
                runClient(SERVER_ADDRESS, SERVER_PORT);
            }
            outputStream.write(partOfFile);
            fileLength -= size;
            dataSize += size;
            socketWriter.writeByte(6);
            socketWriter.flush();
        }
        isNeedStop = true;
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

    private void uploadAfterConnectionIssue(DataOutputStream socketWriter,
                                            DataInputStream socketReader) throws IOException, InterruptedException {
        String lastFileName = readMessage(socketReader);
        int leftFileLength = readLength(socketReader);
        byte[] file = readFile(WORK_DIRECTORY_PATH + lastFileName);
        writeFile(socketWriter, socketReader,
                Arrays.copyOfRange(file, file.length - leftFileLength, file.length));
        System.out.println("Response: " + readMessage(socketReader));
    }


    public void runClient(String address, int port) {
        try {
            socket = new Socket(address, port);
            System.out.println("Connected");

            DataOutputStream socketWriter = new DataOutputStream(socket.getOutputStream());
            DataInputStream socketReader = new DataInputStream(
                    new BufferedInputStream(socket.getInputStream()));

            System.out.println("Response from server: " + readMessage(socketReader));

            writeInSocket(socketWriter, "Client1");
            String lastCommand = readMessage(socketReader);
            if (!lastCommand.isEmpty() && lastCommand.equals("upload")) {
                uploadAfterConnectionIssue(socketWriter, socketReader);
            }
            if (!lastCommand.isEmpty() && lastCommand.equals("download")) {
                String lastFileName = readMessage(socketReader);
                int fileLength = readFile(WORK_DIRECTORY_PATH + lastFileName).length;
                writeLength(socketWriter, fileLength);
                int leftFileLength = readLength(socketReader);
                saveFile(socketReader, socketWriter, WORK_DIRECTORY_PATH + lastFileName, leftFileLength);
                System.out.println("Response: " + readMessage(socketReader));
            }
            String line = "";
            while (!line.equals("exit\n")) {
                System.out.print("Enter command: ");
                line = scanner.nextLine();
                char delimeter = '\n';
                line += new String(new byte[]{(byte) delimeter});
                writeInSocket(socketWriter, line);
                String[] words = line.split("\\s"); //пробел
                switch (words[0].toLowerCase()) {
                    case "upload":

                        if (new File(WORK_DIRECTORY_PATH + words[1]).exists()) {
                            byte[] file = readFile(WORK_DIRECTORY_PATH + words[1]);
                            writeLength(socketWriter, file.length);
                            writeFile(socketWriter, socketReader, file);
                        } else {
                            System.out.println("File not found");
                            writeLength(socketWriter, 0);
                            continue;
                        }
                        break;
                    case "download":
                        int fileLength = readLength(socketReader);
                        if (fileLength != 0) {
                            saveFile(socketReader, socketWriter, WORK_DIRECTORY_PATH + words[1], fileLength);
                        }
                        break;
                }
                System.out.println("Response from server: " + readMessage(socketReader));
            }
            closeConnection(socket, socketReader, socketWriter);
        } catch (IOException | InterruptedException u) {
            u.printStackTrace();
        }
    }

    public static void main(String[] args) {
        new Client().runClient(SERVER_ADDRESS, SERVER_PORT);
    }
}
