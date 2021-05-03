package com.example.restservice;

import org.springframework.stereotype.Component;

@Component
public class MathLab {

    public static MathLab math = new MathLab();

    private String maxRes;
    private String minRes;
    private String avgRes;
    private String maxA;

    MathLab() {
        maxRes = "Incorrect data\n";
        minRes = "Incorrect data\n";
        avgRes = "Incorrect data\n";
        maxA = "Incorrect data\n";
    }

    MathLab(String a, String b, String c, String maxa) {
        this.maxRes = a;
        this.minRes = b;
        this.avgRes = c;
        this.maxA = maxa;
    }

    public String setMax(String max) {
        return this.maxRes = max;
    }

    public String setMin(String min) {
        return this.minRes = min;
    }

    public String setAvg(String avg) {
        return this.avgRes = avg;
    }

    public String setMaxA(String a) {
        return this.maxA = a;
    }

    public String getMax() {
        return this.maxRes;
    }

    public String getMin() {
        return this.minRes;
    }

    public String getAvg() {
        return this.avgRes;
    }

    public String getMaxA() {
        return this.maxA;
    }


}
