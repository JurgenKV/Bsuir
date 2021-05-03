package com.example.restservice;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.springframework.web.bind.annotation.RestController;


@RestController
public class Counter extends Thread {
    Integer count = 0;
    Boolean countTrue = true;

    Counter() {
        start();
    }

    public void setCountTrue() {
        this.countTrue = true;
    }

    public void setCountFalse() {
        this.countTrue = false;
    }

    public Integer getCounter() {
        return this.count;
    }

    public synchronized void run() {
        while (true) {

            try {
                Semaphor.sem.acquire(1);
                //setCountFalse();
                Logger LOGGER;
                LOGGER = Logger.getLogger(Controller.class.getName());
                count++;
                LOGGER.log(Level.INFO, "Счетчик свое отработал O_o", count);
                System.out.println(count);

            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}



