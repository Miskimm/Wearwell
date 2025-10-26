package com.example.bluetooth_869_10N_EN.protocol;

import static com.example.bluetooth_869_10N_EN.utils.CommonUtil.isNull;

public class Protocol {

    public static final String HEAR_WARN = "hear_warn";
    public static final String SPO2_WARN = "spo2_warn";
    public static final String ALCO_WARN = "alco_warn";
    public static final String TIME_WARN = "time_warn";
    public static final String HEART = "$heart:";
    public static final String SPO2 = "$spo2:";
    public static final String ALCOHOL = "$alcohol:";
    public static final String STA_OFF = "$sta=off";
    public static final String STA_ON = "$sta=on";
    public static final String TIME = "$time:";
    public static final String WARN_1 = "warn1";
    public static final String WARN_2 = "warn2";

    public static final byte[] OPEN = {'o', 'p', 'e', 'n', 0x0d, 0x0a};
    public static final byte[] CLOSE = {'c','l','o','s','e', 0x0d, 0x0a};

    public static byte[] assembleSendBosMsg(String str0, String str1, String str2, String str3) {
        if (isNull(str0) || isNull(str1) || isNull(str2) || isNull(str3)) return null;

        byte[] bos1 = str0.getBytes();
        byte[] bos2 = str1.getBytes();
        byte[] bos3 = str2.getBytes();
        byte[] bos4 = str3.getBytes();

        byte[] send_bos = new byte[bos1.length + bos2.length + bos3.length + bos4.length + 45];

        int i=0;
        int n=0;

        send_bos[i++] = 'H';
        send_bos[i++] = 'e';
        send_bos[i++] = 'a';
        send_bos[i++] = 'r';
        send_bos[i++] = 't';
        send_bos[i++] = 'M';
        send_bos[i++] = 'i';
        send_bos[i++] = 'n';
        send_bos[i++] = ':';

        n = i;
        for (i = 0; i < bos1.length; i++) {
            send_bos[n + i] = bos1[i];
        }
        n += i;
        send_bos[n++] = ',';
        send_bos[n++] = 'H';
        send_bos[n++] = 'e';
        send_bos[n++] = 'a';
        send_bos[n++] = 'r';
        send_bos[n++] = 't';
        send_bos[n++] = 'M';
        send_bos[n++] = 'a';
        send_bos[n++] = 'x';
        send_bos[n++] = ':';

        for (i = 0; i < bos2.length; i++) {
            send_bos[n + i] = bos2[i];
        }
        n += i;
        send_bos[n++] = ',';
        send_bos[n++] = 'O';
        send_bos[n++] = 'x';
        send_bos[n++] = 'y';
        send_bos[n++] = 'g';
        send_bos[n++] = 'e';
        send_bos[n++] = 'n';
        send_bos[n++] = 'M';
        send_bos[n++] = 'i';
        send_bos[n++] = 'n';
        send_bos[n++] = ':';

        for (i = 0; i < bos3.length; i++) {
            send_bos[n + i] = bos3[i];
        }
        n += i;
        send_bos[n++] = ',';
        send_bos[n++] = 'A';
        send_bos[n++] = 'l';
        send_bos[n++] = 'c';
        send_bos[n++] = 'o';
        send_bos[n++] = 'h';
        send_bos[n++] = 'o';
        send_bos[n++] = 'l';
        send_bos[n++] = 'M';
        send_bos[n++] = 'a';
        send_bos[n++] = 'x';
        send_bos[n++] = ':';

        for (i = 0; i < bos4.length; i++) {
            send_bos[n + i] = bos4[i];
        }
        n += i;

        send_bos[n++] = 0x0d;
        send_bos[n++] = 0x0a;
        return send_bos;
    }

}
