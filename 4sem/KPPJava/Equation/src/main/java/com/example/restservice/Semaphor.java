package com.example.restservice;


import org.springframework.stereotype.Component;

import java.util.concurrent.Semaphore;

@Component
public class Semaphor {
    public static final Semaphore sem = new Semaphore(1, true);
}
