package com.example.restservice;

import org.springframework.web.bind.annotation.GetMapping;

public class CountController {
    Counter count;
    @GetMapping("/equation/count")
    public Integer eq() throws InterruptedException {
        return count.getCounter();
    }
}
