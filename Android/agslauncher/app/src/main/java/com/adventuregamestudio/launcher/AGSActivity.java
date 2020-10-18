package com.adventuregamestudio.launcher;

import android.app.AlertDialog;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.util.Log;

import org.libsdl.app.SDLActivity;

import android.Manifest;

import android.content.Intent;
import androidx.annotation.Keep;
import androidx.annotation.RequiresApi;
import androidx.core.app.ActivityCompat;

import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class AGSActivity extends SDLActivity {
    private static String gamePath = "";
    protected final int[] externalStorageRequestDummy = new int[1];
    public static final int EXTERNAL_STORAGE_REQUEST_CODE = 2;
    private static Boolean handledIntent = false;

    @Override
    protected String[] getLibraries() {
        return new String[] {
                "hidapi",
                "SDL2",
                "engine",
                "ags"
        };
    }


    /**
     * This method is called by SDL before starting the native application thread.
     * It can be overridden to provide the arguments after the application name.
     * The default implementation returns an empty array. It never returns null.
     * @return arguments for the native application.
     */
    @Override
    protected String[] getArguments() {
        if(!handledIntent && gamePath.isEmpty())
        {

        }

        String[] args  = new String[1];
        args[0] = getGamePath();
        return args;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("GameActivity", "started");

        // must be reset or it will use the existing value.
        //gamePath = "";

        // Get filename from "Open with" of another application
        handleIntent(this.getIntent());

        super.onCreate(savedInstanceState);
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public static String getFullPathFromContentUri(final Context context, final Uri uri) {

        final boolean isKitKat = Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;

        // DocumentProvider
        if (isKitKat && DocumentsContract.isDocumentUri(context, uri)) {
            // ExternalStorageProvider
            if ("com.android.externalstorage.documents".equals(uri.getAuthority())) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];

                if ("primary".equalsIgnoreCase(type)) {
                    return Environment.getExternalStorageDirectory() + "/" + split[1];
                }

                // TODO handle non-primary volumes
            }
            // DownloadsProvider
            else if ("com.android.providers.downloads.documents".equals(uri.getAuthority())) {

                final String id = DocumentsContract.getDocumentId(uri);
                final Uri contentUri = ContentUris.withAppendedId(
                        Uri.parse("content://downloads/public_downloads"), Long.valueOf(id));

                return getDataColumn(context, contentUri, null, null);
            }
            // MediaProvider
            else if ("com.android.providers.media.documents".equals(uri.getAuthority())) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];

                Uri contentUri = null;
                if ("image".equals(type)) {
                    contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                } else if ("video".equals(type)) {
                    contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                } else if ("audio".equals(type)) {
                    contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                }

                final String selection = "_id=?";
                final String[] selectionArgs = new String[]{
                        split[1]
                };

                Cursor cursor = null;
                final String column = "_data";
                final String[] projection = {
                        column
                };

                try {
                    cursor = context.getContentResolver().query(uri, projection, selection, selectionArgs,
                            null);
                    if (cursor != null && cursor.moveToFirst()) {
                        final int column_index = cursor.getColumnIndexOrThrow(column);
                        return cursor.getString(column_index);
                    }
                } finally {
                    if (cursor != null)
                        cursor.close();
                }
                return null;
            }
        }
        // MediaStore (and general)
        else if ("content".equalsIgnoreCase(uri.getScheme())) {
            return getDataColumn(context, uri, null, null);
        }
        // File
        else if ("file".equalsIgnoreCase(uri.getScheme())) {
            return uri.getPath();
        }

        return null;
    }

    private static String getDataColumn(Context context, Uri uri, String selection,
                                        String[] selectionArgs) {

        Cursor cursor = null;
        final String column = "_data";
        final String[] projection = {
                column
        };

        try {
            cursor = context.getContentResolver().query(uri, projection, selection, selectionArgs,
                    null);
            if (cursor != null && cursor.moveToFirst()) {
                final int column_index = cursor.getColumnIndexOrThrow(column);
                return cursor.getString(column_index);
            }
        } finally {
            if (cursor != null)
                cursor.close();
        }
        return null;
    }

    @Override
    protected void onNewIntent(Intent intent) {
        Log.d("GameActivity", "onNewIntent() with " + intent);
        handleIntent(intent);
        onCreate(new Bundle());
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    protected void handleIntent(Intent intent) {
        Uri game = null;

        // Try to handle "Share" intent.
        // This is actually "bit tricky" to get working in user phone
        // because shared static variables issue in the native side
        // (user have to clear LOVE for Android in their recent apps list).
        if (Intent.ACTION_SEND.equals(intent.getAction())) {
            game = (Uri) intent.getParcelableExtra(Intent.EXTRA_STREAM);
        } else {
            game = intent.getData();
        }

        if (game != null) {
            String scheme = game.getScheme();
            String path = game.getPath();
            // If we have a game via the intent data we we try to figure out how we have to load it. We
            // support the following variations:
            if (scheme.equals("file")) {

                Log.d("GameActivity", "Received file:// intent with path: " + path);
                // If we were given the path of a main.lua then use its
                // directory. Otherwise use full path.
                List<String> path_segments = game.getPathSegments();
                File file = new File(path);
                File[] agsgames = findAGSGamesInDir(game);

                if (agsgames.length > 0) {
                    gamePath = getFullPathFromContentUri(this.getApplicationContext(), Uri.parse(agsgames[0].getParent()));
                    handledIntent = true;
                } else {
                    gamePath = getFullPathFromContentUri(this.getApplicationContext(), Uri.parse(path));
                    gamePath = path;
                    handledIntent = true;
                }
            } else if (scheme.equals("content")) {
                Log.d("GameActivity", "Received content:// intent with path: " + path);
                try {

                    File file = new File(path);
                    File[] agsgames = findAGSGamesInDir(game);

                    if (agsgames.length > 0) {
                        String gamename =  agsgames[0].getName();
                        File folder = new File(agsgames[0].getParent());
                        Log.d("GameActivity", "Files in dir:" + folder.toString());
                        File[] allFiles = folder.listFiles(new FileFilter() {
                            @Override
                            public boolean accept(File pathname) {
                                String name = pathname.getName().toLowerCase();
                                return name.endsWith(".vox") ||
                                        name.endsWith(".ags") ||
                                        name.endsWith(".exe") ||
                                        name.endsWith(".tra") ||
                                        name.endsWith(".ogv") ||
                                        name.endsWith(".001") ||
                                        name.endsWith(".002") ||
                                        name.endsWith(".003") ||
                                        name.endsWith(".004") ||
                                        name.endsWith(".005") ||
                                        name.endsWith(".006") ||
                                        name.endsWith(".007") ||
                                        name.endsWith(".008") ||
                                        name.endsWith(".009") ||
                                        name.endsWith(".cfg") && pathname.isFile();
                            }});

                        if(allFiles != null && allFiles.length > 0)
                        {
                            for(int i=0; i<allFiles.length; i++)
                            {
                                File f = allFiles[i];
                                String fname;

                                if(f != null && f.getName().equals(gamename)) continue;

                                String[] pathSegments = f.getPath().split("/");
                                if (pathSegments != null && pathSegments.length > 0) {
                                    fname = pathSegments[pathSegments.length - 1];

                                    String destination_f = this.getCacheDir().getPath() + "/" + fname;
                                    InputStream data = getContentResolver().openInputStream(Uri.fromFile(f));

                                    Log.d("GameActivity", "file in asset:" + fname);

                                    copyAssetFile(data, destination_f);
                                }

                            }
                        }

                        String filename =  agsgames[0].getName();
                        String[] pathSegments = path.split("/");
                        if (pathSegments != null && pathSegments.length > 0) {
                            filename = pathSegments[pathSegments.length - 1];
                        }

                        String destination_file = this.getCacheDir().getPath() + "/" + filename;
                        InputStream data = getContentResolver().openInputStream(game);

                        // copyAssetFile automatically closes the InputStream
                        if (copyAssetFile(data, destination_file)) {
                            gamePath = destination_file; // getFullPathFromContentUri(this.getApplicationContext(), Uri.parse(destination_file));
                            handledIntent = true;
                        }


                    }

                } catch (Exception e) {
                    Log.d("GameActivity", "could not read content uri " + game.toString() + ": " + e.getMessage());
                }
            } else {
                Log.e("GameActivity", "Unsupported scheme: '" + game.getScheme() + "'.");

                AlertDialog.Builder alert_dialog = new AlertDialog.Builder(this);
                alert_dialog.setMessage("Could not load AGS game '" + path
                        + "' as it uses unsupported scheme '" + game.getScheme()
                        + "'. Please contact the developer.");
                alert_dialog.setTitle("AGS for Android Error");
                alert_dialog.setPositiveButton("Exit",
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int id) {
                                finish();
                            }
                        });
                alert_dialog.setCancelable(false);
                alert_dialog.create().show();
            }
        }

        Log.d("GameActivity", "new gamePath: " + gamePath);
    }


    public File[] findAGSGamesInDir(Uri fileDir) {
        Log.d("GameActivity", "received dir: " + fileDir.toString());
        File dirPath =  new File(getFullPathFromContentUri(this.getApplicationContext(), fileDir));

        Log.d("GameActivity", "dirPath as string: " + dirPath.toString());


        if(dirPath.toString().endsWith(".exe") || dirPath.toString().endsWith(".ags"))
        {
            Log.d("GameActivity", "file ends with .exe or .ags");
            File[] ret = new File[1];
            ret[0] = dirPath;
            return ret;
        }

        File[] listFiles =dirPath.listFiles();

        ArrayList<File> listFilesOut = new ArrayList<File>();

        Log.d("GameActivity", "listFiles.length " + listFiles.length);

        for (int i = 0; i < listFiles.length; i++) {

            if (listFiles[i].isFile()) {
                String fileName = listFiles[i].getName();
                if (fileName.endsWith(".ags") || fileName.endsWith(".exe")) {
                    listFilesOut.add(listFiles[i]);
                }
            }
        }

        File[] out = (File[]) listFilesOut.toArray();

        return out;
    }

    protected void checkGameFolder() {
        // If no game.exe or game.ags was found , fall back to the game in
        // <external storage>/Android/data/<package name>/games/agsgame
        Log.d("GameActivity", "fallback to agsgame folder");
        File ext = getExternalFilesDir("games");
        if ((new File(ext, "/agsgame/")).exists()) {
            gamePath = ext.getPath() + "/agsgame/";
        }
    }

    @Keep
    public static String getGamePath() {
        AGSActivity self = (AGSActivity) mSingleton; // use SDL provided one
        Log.d("AGSActivity", "called getGamePath(), game path = " + gamePath);

        if (gamePath.length() > 0) {
            if(self.hasExternalStoragePermission()) {
                return gamePath;
            } else {
                Log.d("GameActivity", "cannot open game " + gamePath + ": no external storage permission given!");
            }
        } else {
            self.checkGameFolder();
            if (gamePath.length() > 0)
                return gamePath;
        }

        return "";
    }


    /**
     * Copies a given file from the assets folder to the destination.
     *
     * @return true if successful
     */
    boolean copyAssetFile(InputStream source_stream, String destinationFileName) {
        boolean success = false;

        BufferedOutputStream destination_stream = null;
        try {
            destination_stream = new BufferedOutputStream(new FileOutputStream(destinationFileName, false));
        } catch (IOException e) {
            Log.d("GameActivity", "Could not open destination file: " + e.getMessage());
        }

        // perform the copying
        int chunk_read = 0;
        int bytes_written = 0;

        assert (source_stream != null && destination_stream != null);

        try {
            byte[] buf = new byte[1024];
            chunk_read = source_stream.read(buf);
            do {
                destination_stream.write(buf, 0, chunk_read);
                bytes_written += chunk_read;
                chunk_read = source_stream.read(buf);
            } while (chunk_read != -1);
        } catch (IOException e) {
            Log.d("GameActivity", "Copying failed:" + e.getMessage());
        }

        // close streams
        try {
            if (source_stream != null) source_stream.close();
            if (destination_stream != null) destination_stream.close();
            success = true;
        } catch (IOException e) {
            Log.d("GameActivity", "Copying failed: " + e.getMessage());
        }

        Log.d("GameActivity", "Successfully copied stream to " + destinationFileName + " (" + bytes_written + " bytes written).");
        return success;
    }


    public void showExternalStoragePermissionMissingDialog() {
        AlertDialog dialog = new AlertDialog.Builder(mSingleton)
                .setTitle("Storage Permission Missing")
                .setMessage("AGS for Android will not be able to run non-packaged games without storage permission.")
                .setNeutralButton("Continue", null)
                .create();
        dialog.show();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (grantResults.length > 0) {
            Log.d("GameActivity", "Received a request permission result");

            switch (requestCode) {
                case EXTERNAL_STORAGE_REQUEST_CODE: {
                    if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                        Log.d("GameActivity", "Permission granted");
                    } else {
                        Log.d("GameActivity", "Did not get permission.");
                        if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.READ_EXTERNAL_STORAGE)) {
                            showExternalStoragePermissionMissingDialog();
                        }
                    }

                    Log.d("GameActivity", "Unlocking AGS thread");
                    synchronized (externalStorageRequestDummy) {
                        externalStorageRequestDummy[0] = grantResults[0];
                        externalStorageRequestDummy.notify();
                    }
                    break;
                }
                default:
                    super.onRequestPermissionsResult(requestCode, permissions, grantResults);
            }
        }
    }


    @Keep
    public boolean hasExternalStoragePermission() {
        if (ActivityCompat.checkSelfPermission(this,
                Manifest.permission.READ_EXTERNAL_STORAGE)
                == PackageManager.PERMISSION_GRANTED) {
            return true;
        }

        Log.d("GameActivity", "Requesting permission and locking AGS thread until we have an answer.");
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, EXTERNAL_STORAGE_REQUEST_CODE);

        synchronized (externalStorageRequestDummy) {
            try {
                externalStorageRequestDummy.wait();
            } catch (InterruptedException e) {
                Log.d("GameActivity", "requesting external storage permission", e);
                return false;
            }
        }

        return ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
    }

}
