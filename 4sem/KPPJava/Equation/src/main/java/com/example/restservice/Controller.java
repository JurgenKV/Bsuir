package com.example.restservice;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class Controller
{
    @GetMapping("/equation")
    public Equation eq(@RequestParam(value = "a" , defaultValue = "-1")double a,
                       @RequestParam(value = "b" , defaultValue = "0")double b,
                       @RequestParam(value = "n" , defaultValue = "2")int n,
                       String result)
    {
        return new Equation(a,b,n,result);
    }
}