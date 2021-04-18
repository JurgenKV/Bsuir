package com.example.restservice;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Component;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import static sun.jvm.hotspot.runtime.PerfMemory.start;

@RestController
public class Counter extends Thread {
    Integer count = 0;
    Boolean countTrue = true;
  //  Semaphor sema;
/*
    @GetMapping("/equation/count")
    public Integer eq() throws InterruptedException {
        return getCounter();
    }
*/
    Counter() {
       // super();
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



