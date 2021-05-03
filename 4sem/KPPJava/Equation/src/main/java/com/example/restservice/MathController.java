package com.example.restservice;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.beans.factory.annotation.Autowired;

@RestController
public class MathController {
    @Autowired
    MathLab math;

    @GetMapping("/equation/math")
    public MathLab eq() throws InterruptedException {
        return math;
    }
}
