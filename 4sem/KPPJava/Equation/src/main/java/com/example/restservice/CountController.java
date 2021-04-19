package com.example.restservice;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;


@RestController
public class CountController {
     @Autowired
    Counter count;

    @GetMapping("/equation/count")
    public Integer eq() throws InterruptedException {
        return count.getCounter();
    }
}
