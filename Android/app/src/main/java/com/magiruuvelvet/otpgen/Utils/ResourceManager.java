package com.magiruuvelvet.otpgen.Utils;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.content.res.Resources;
import android.util.Log;

public class ResourceManager
{
    public static void createFile(final String outputFile, final Context context, final Integer inputRawResource)
        throws IOException
    {
        final OutputStream outputStream = new FileOutputStream(outputFile);

        final Resources resources = context.getResources();
        final byte[] largeBuffer = new byte[1024 * 4];
        int totalBytes = 0;
        int bytesRead = 0;

        final InputStream inputStream = resources.openRawResource(inputRawResource);
        while ((bytesRead = inputStream.read(largeBuffer)) > 0)
        {
            if (largeBuffer.length == bytesRead)
            {
                outputStream.write(largeBuffer);
            }
            else
            {
                final byte[] shortBuffer = new byte[bytesRead];
                System.arraycopy(largeBuffer, 0, shortBuffer, 0, bytesRead);
                outputStream.write(shortBuffer);
            }
            totalBytes += bytesRead;
        }
        inputStream.close();

        outputStream.flush();
        outputStream.close();
    }
}
