package com.example.restservice;

import java.util.HashMap;
import java.util.List;

import org.springframework.stereotype.Component;

@Component
public class CacheMap {

    private HashMap<String, Equation> CacheMap;

    public CacheMap() {
        CacheMap = new HashMap<String, Equation>();
    }

    public void insert(String key, Equation value) {
        CacheMap.put(key, value);
        System.out.println(CacheMap.get(key));
    }

    public Equation getKey(String key) {
        return (Equation) CacheMap.get(key);
    }

    boolean isStored(List<String> id) {
        for (int i = 0; i < id.size(); i++) {
            String idKey = id.get(i);
            if (CacheMap.containsKey(idKey)) {
                return true;
            }
        }
        return false;
        // return CacheMap.containsKey(idKey);
    }

    boolean isStored(Equation id) {
        String idKey = id.getKeyEq();
        return CacheMap.containsKey(idKey);
        // return CacheMap.containsKey(idKey);
    }


}