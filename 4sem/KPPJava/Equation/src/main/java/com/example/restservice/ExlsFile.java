package com.example.restservice;

import org.springframework.stereotype.Component;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.logging.Level;

@Component
public class ExlsFile {
    public static final File myFile = new File("d:\\Study\\Programming\\Java\\Equation\\File.xls");
}
