package com.company;

import java.io.*;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.text.DateFormat;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.Iterator;
import java.util.Set;
import java.util.stream.Collectors;

public class Server {

    private String lastClient = "";
    private String lastCommand = "";
    private String lastFileName = "";
    private int lastPosition = 0;
    private long lastFileLength = 0;

    private static final String DIRECTORY_PATH = "D:\\work_directory_for_server\\";

    public void runServer() {
        try {
            InetAddress host = InetAddress.getByName("172.20.10.2");
            Selector selector = Selector.open();
            ServerSocketChannel serverSocketChannel = ServerSocketChannel.open();
            serverSocketChannel.configureBlocking(false);
            serverSocketChannel.bind(new InetSocketAddress(host, 1234));
            serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
            System.out.println("Server started...");
            SelectionKey key = null;
            while (true) {
                if (selector.select() <= 0)
                    continue;
                Set<SelectionKey> selectedKeys = selector.selectedKeys();
                Iterator<SelectionKey> iterator = selectedKeys.iterator();
                while (iterator.hasNext()) {
                    key = iterator.next();
                    iterator.remove();
                    if (key.isAcceptable()) {
                        SocketChannel sc = serverSocketChannel.accept();
                        sc.configureBlocking(false);
                        sc.register(selector, SelectionKey.OP_READ);
                        System.out.println("Connection Accepted: " + sc.getLocalAddress());
                    }
                    if (key.isReadable()) {
                        SocketChannel sc = (SocketChannel) key.channel();
                        ByteBuffer bb = ByteBuffer.allocate(1024);
                        try {
                            sc.read(bb);
                        } catch (IOException ignored) {
                        }
                        String request = new String(bb.array()).trim();
                        if (request.length() <= 0) {
                            sc.close();
                            System.out.println("Connection closed...");
                            System.out.println("Server will keep running. " + "Try running another client to " + "re-establish connection");
                        }
                        checkLastCommand(sc);
                        parseCommand(request, sc, key);
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void checkLastCommand(SocketChannel sc) {
        if (lastCommand.equals("upload") && !lastFileName.isEmpty()) {
            reUpload(sc);
            lastFileName = "";
            lastClient = "";
            lastPosition = 0;
            lastFileLength = 0;
        }
        if (lastCommand.equals("download") && !lastFileName.isEmpty()) {
            reDownload(sc);
            lastFileName = "";
            lastClient = "";
            lastPosition = 0;
            lastFileLength = 0;
        }
    }

    private void reDownload(SocketChannel sc) {
        try {
            FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + lastFileName, "r").getChannel();

            long fileLength = fileChannel.size();
            long start = System.nanoTime();

            int position = lastPosition;
            while (position < fileLength) {
                position += fileChannel.transferTo(position, 1024, sc);
            }

            long end = System.nanoTime();
            long difference = end - start;
            DecimalFormat decimalFormat = new DecimalFormat("#.0#");
            System.out.println("Bitrate (b/s): " + decimalFormat.format(fileLength / (difference / 1000000000.0)));
            System.out.println("File " + lastFileName + " successfully downloaded for " + lastClient);

            fileChannel.close();
        } catch (IOException exception) {
            exception.printStackTrace();
        }
    }

    private void reUpload(SocketChannel sc) {
        try {
            FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + lastFileName, "rw").getChannel();


            long start = System.nanoTime();
            int position = lastPosition;
            while (position < lastFileLength) {
                position += fileChannel.transferFrom(sc, position, 1024);
            }

            System.out.println("File " + lastFileName + " successfully received and saved from " + lastClient);
            long end = System.nanoTime();
            long difference = end - start;
            DecimalFormat decimalFormat = new DecimalFormat("#.0#");
            System.out.println("Bitrate (b/s): " + decimalFormat.format(lastFileLength / (difference / 1000000000.0)));

            fileChannel.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void parseCommand(String result, SocketChannel sc, SelectionKey key) {
        String[] words = result.split("\\s");
        String command = words[0].toLowerCase();
        lastCommand = command;
        if (!command.isEmpty()) {
            switch (command) {
                case "echo" -> executeEcho(words, sc);
                case "time" -> executeTime(sc);
                case "upload" -> {
                    lastFileName = words[1];
                    lastCommand = "upload";
                    executeUpload(sc, words[1], words[2]);
                    lastFileName = "";
                    lastClient = "";
                    lastPosition = 0;
                    lastFileLength = 0;
                }
                case "download" -> {
                    lastFileName = words[1];
                    lastCommand = "download";
                    executeDownload(sc, words[1]);
                    lastFileName = "";
                    lastClient = "";
                    lastPosition = 0;
                    lastFileLength = 0;
                }
                case "default" -> {
                    System.out.println("Invalid command");
                }
            }
        }
    }

    private void executeDownload(SocketChannel sc, String filename) {
        try {
            lastClient = sc.getLocalAddress().toString();
            lastFileName = filename;
            System.out.println("Download command");
            FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + filename, "r").getChannel();

            long fileLength = fileChannel.size();
            lastFileLength = fileLength;

            ByteBuffer bb = ByteBuffer.wrap(String.valueOf(fileLength).getBytes());
            sc.write(bb);

            long start = System.nanoTime();

            int position = 0;
            lastPosition = position;
            while (position < fileLength) {
                position += fileChannel.transferTo(position, 1024, sc);
                lastPosition = position;
            }

            long end = System.nanoTime();
            long difference = end - start;
            DecimalFormat decimalFormat = new DecimalFormat("#.0#");
            System.out.println("Bitrate (b/s): " + decimalFormat.format(fileLength / (difference / 1000000000.0)));
            System.out.println("File " + filename + " successfully downloaded for " + lastClient);

            fileChannel.close();
        } catch (IOException exception) {
            //exception.printStackTrace();
        }
    }

    private void executeUpload(SocketChannel sc, String filename, String fileLength) {
        try {
            lastClient = sc.getLocalAddress().toString();
            lastFileName = filename;
            System.out.println("Upload command");

            FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + filename, "rw").getChannel();
            long size = Long.parseLong(fileLength.trim());
            lastFileLength = size;


            long start = System.nanoTime();
            int position = 0;
            while (position < size) {
                position += fileChannel.transferFrom(sc, position, 1024);
                lastPosition = position;
            }

            System.out.println("File " + filename + " successfully received and saved from " + lastClient);
            long end = System.nanoTime();
            long difference = end - start;
            DecimalFormat decimalFormat = new DecimalFormat("#.0#");
            System.out.println("Bitrate (b/s): " + decimalFormat.format(size / (difference / 1000000000.0)));

            fileChannel.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static void executeTime(SocketChannel sc) {
        try {
            DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
            Date date = new Date();
            String msg = dateFormat.format(date);
            System.out.println("Time command from " + sc.getLocalAddress().toString() + ": " + msg);
            ByteBuffer bb = ByteBuffer.wrap(msg.getBytes());
            sc.write(bb);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static void executeEcho(String[] words, SocketChannel sc) {
        try {
            String msg = Arrays.stream(words).skip(1).collect(Collectors.joining(" "));
            System.out.println("Echo command from " + sc.getLocalAddress().toString() + ": " + msg);
            ByteBuffer bb = ByteBuffer.wrap(msg.getBytes());
            sc.write(bb);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        new Server().runServer();
    }
}