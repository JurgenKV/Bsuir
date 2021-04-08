package com.example.restservice;

import java.util.HashMap;

import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Component;

@Component
public class CacheMap {

    private HashMap<String, String> CacheMap;

    public CacheMap() {
        CacheMap = new HashMap<String, String>();
    }

    public void insert(String key, String value) {
        CacheMap.put(key, value);
        System.out.println(CacheMap.get(key));
    }

    public String getKey(String key) {
        return (String) CacheMap.get(key);
    }

    boolean isStored(String key) {
        return CacheMap.containsKey(key);
    }
}