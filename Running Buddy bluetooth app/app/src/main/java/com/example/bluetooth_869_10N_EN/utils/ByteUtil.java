package com.example.bluetooth_869_10N_EN.utils;

import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

public class ByteUtil {

    /**
     * int -> hex string，带补零
     * eg: i = 20 -> "14", -10 -> "f6"
     */
    public static String intToHexString(int i) {
        String strHex = Integer.toHexString(i & 0xFF);
        if (i >= 0 && i <= 15) {
            return "0" + strHex;
        } else {
            return strHex;
        }
    }

    /**
     * Hex字符串转整数
     * "60" -> 96
     */
    public static int hexStringToInt(String hex) {
        if (hex == null || hex.isEmpty()) return -1;
        try {
            return Integer.parseInt(hex, 16);
        } catch (NumberFormatException e) {
            return -1;
        }
    }

    /**
     * 字符串转 Hex（ASCII）
     * "1" -> "31"
     * "10" -> "3130"
     */
    public static String strToHexString(String str) {
        StringBuilder hexStr = new StringBuilder();
        for (char ch : str.toCharArray()) {
            String hex = Integer.toHexString(ch);
            hexStr.append(hex);
        }
        return hexStr.toString();
    }

    /**
     * Hex字符串转 ASCII 字符串
     * "7D" -> "}"
     */
    public static String hexStr2Str(String hexStr) {
        if (hexStr == null || hexStr.isEmpty()) return "";
        hexStr = hexStr.trim().toUpperCase(Locale.getDefault());
        int len = hexStr.length();

        if (len % 2 != 0) {
            return ""; // 长度必须是偶数
        }

        byte[] bytes = new byte[len / 2];
        String str = "0123456789ABCDEF";

        for (int i = 0; i < bytes.length; i++) {
            int high = str.indexOf(hexStr.charAt(2 * i));
            int low = str.indexOf(hexStr.charAt(2 * i + 1));
            int value = (high << 4) + low;
            bytes[i] = (byte) (value & 0xff);
        }

        try {
            return new String(bytes, Charset.forName("ASCII"));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return new String(bytes);
    }

    /**
     * Hex字符串转数值（支持大写和小写）
     * "60" -> 96
     */
    public static int hexStr2Int(String hex) {
        if (hex == null || hex.isEmpty()) return 0;

        Map<Character, Integer> map = new HashMap<>();
        for (int i = 0; i <= 9; i++) {
            map.put((char) ('0' + i), i);
        }
        for (int j = 10; j <= 15; j++) {
            map.put((char) ('A' + (j - 10)), j);
            map.put((char) ('a' + (j - 10)), j);
        }

        hex = hex.toUpperCase(Locale.getDefault());
        int number = 0;
        for (int i = 0; i < hex.length(); i++) {
            char c = hex.charAt(i);
            Integer val = map.get(c);
            if (val == null) continue;
            number = number * 16 + val;
        }
        return number;
    }

    /**
     * 合并两个字节为一个整型（高位在前）
     */
    public static int mergeByte2Hex(byte high, byte low) {
        return ((high & 0xFF) << 8) | (low & 0xFF);
    }

    /**
     * 将一个整数转换为两个字节的数组（高位在前）
     */
    public static byte[] intToByteArray(int value) {
        byte[] mValue = new byte[2];
        mValue[0] = (byte) ((value >> 8) & 0xFF);
        mValue[1] = (byte) (value & 0xFF);
        return mValue;
    }
}