package com.company;

import java.io.*;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.Iterator;
import java.util.Set;

public class Client {
    private static BufferedReader input = null;
    private boolean flag = true;
    private static final String DIRECTORY_PATH = "D:\\work_directory_for_client\\";
    private String lastCommand = "";
    private String lastFileName = "";
    private int lastPosition = 0;
    private long lastFileLength = 0;
    private String tempFilename = "";
    private String tempCommand = "";
    private long tempFileLength = 0;

    public void runClient() {
        try {
            InetSocketAddress addr = new InetSocketAddress(InetAddress.getByName("192.168.100.5"), 1234);
            Selector selector = Selector.open();
            SocketChannel sc = SocketChannel.open();
            sc.configureBlocking(false);
            sc.connect(addr);
            sc.register(selector, SelectionKey.OP_CONNECT | SelectionKey.OP_READ | SelectionKey.OP_WRITE);
            input = new BufferedReader(new InputStreamReader(System.in));
            while (true) {
                if (selector.select() > 0) {
                    Boolean doneStatus = processReadySet(selector.selectedKeys());
                    if (doneStatus) {
                        break;
                    }
                }
            }
            sc.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Boolean processReadySet(Set readySet) {
        try {

            SelectionKey key = null;
            Iterator iterator = null;
            iterator = readySet.iterator();
            while (iterator.hasNext()) {
                key = (SelectionKey) iterator.next();
                iterator.remove();
            }
            if (key.isConnectable()) {
                Boolean connected = processConnect(key);
                if (!connected) {
                    return true;
                }
            }
            if (key.isReadable()) {
                SocketChannel sc = (SocketChannel) key.channel();
                if (!"".equals(tempFilename)) {
                    getSize(sc);
                    if (!"".equals(tempCommand)) {
                        getFileFromServer(sc, tempFilename);
                        tempFilename = "";
                        tempCommand = "";
                    }
                } else {
                    ByteBuffer bb = ByteBuffer.allocate(1024);
                    sc.read(bb);
                    String result = new String(bb.array()).trim();
                    System.out.println("Message received from Server: " + result);
                }
                flag = !flag;
            }
            if (key.isWritable()) {
                if (flag) {
                    System.out.print("Type a message (type quit to stop): ");
                    String command = input.readLine();
                    if (command.equalsIgnoreCase("quit")) {
                        return true;
                    }
                    command = addLength(command);
                    SocketChannel sc = (SocketChannel) key.channel();
                    ByteBuffer bb = ByteBuffer.wrap(command.getBytes());
                    sc.write(bb);
                    checkLastCommand(sc);
                    parseCommand(command, sc);
                    flag = !flag;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    private String addLength(String command) {
        String[] words = command.split("\\s");
        if ("upload".equals(words[0])) {
            try {
                FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + words[1], "r").getChannel();
                long fileLength = fileChannel.size();
                command = command + " " + fileLength;
                lastFileLength = fileLength;
                fileChannel.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return command;
    }

    private void checkLastCommand(SocketChannel sc) {
        if (lastCommand.equals("upload") && !"".equals(lastFileName)) {
            reUpload(sc);
            lastFileName = "";
            lastPosition = 0;
            lastFileLength = 0;
        }
        if (lastCommand.equals("download") && !"".equals(lastFileName)) {
            reDownload(sc);
            lastFileName = "";
            lastPosition = 0;
            lastFileLength = 0;
        }
    }

    private void reDownload(SocketChannel sc) {
        try {
            FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + lastFileName, "rw").getChannel();


            int position = lastPosition;
            while (position < lastFileLength) {
                position += fileChannel.transferFrom(sc, position, 1024);
            }

            System.out.println("File " + lastFileName + " successfully re-downloaded");
            fileChannel.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void reUpload(SocketChannel sc) {
        try {
            FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + lastFileName, "r").getChannel();

            long fileLength = fileChannel.size();

            int position = lastPosition;
            while (position < fileLength) {
                position += fileChannel.transferTo(position, 1024, sc);
            }

            System.out.println("File " + lastFileName + " successfully re-uploaded");
            fileChannel.close();

        } catch (IOException exception) {
            exception.printStackTrace();
        }
    }

    private void parseCommand(String result, SocketChannel sc) {
        String[] words = result.split("\\s");
        String command = words[0].toLowerCase();
        lastCommand = command;
        if (!command.isEmpty()) {
            switch (command) {
                case "upload" -> {
                    sendFileToServer(sc, words[1]);
                    lastPosition = 0;
                    lastFileName = "";
                    lastFileLength = 0;
                }
                case "download" -> {
                    tempFilename = words[1];
                    tempCommand = command;
                }
            }
        }
    }

    private void getFileFromServer(SocketChannel sc, String filename) {
        try {
            lastFileName = filename;
            lastFileLength = tempFileLength;
            FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + filename, "rw").getChannel();

            int position = 0;
            while (position < tempFileLength) {
                position += fileChannel.transferFrom(sc, position, 1024);
                lastPosition = position;
            }

            System.out.println("File " + filename + " successfully downloaded");
            fileChannel.close();

            lastPosition = 0;
            lastFileName = "";
            lastFileLength = 0;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void getSize(SocketChannel sc) {
        try {
            ByteBuffer bb = ByteBuffer.allocate(1024);
            sc.read(bb);
            String size = new String(bb.array()).trim();
            tempFileLength = Long.parseLong(size);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    private void sendFileToServer(SocketChannel sc, String filename) {
        try {

            lastFileName = filename;
            FileChannel fileChannel = new RandomAccessFile(DIRECTORY_PATH + filename, "r").getChannel();

            long fileLength = fileChannel.size();
            lastFileLength = fileLength;

            int position = 0;
            while (position < fileLength) {
                position += fileChannel.transferTo(position, 1024, sc);
                lastPosition = position;
            }
            System.out.println("File " + filename + " successfully uploaded");
            fileChannel.close();
            flag = !flag;

        } catch (IOException exception) {
            exception.printStackTrace();
        }
    }

    public Boolean processConnect(SelectionKey key) {
        SocketChannel sc = (SocketChannel) key.channel();
        try {
            while (sc.isConnectionPending()) {
                sc.finishConnect();
            }
        } catch (IOException e) {
            key.cancel();
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public static void main(String[] args) {
        new Client().runClient();
    }
}