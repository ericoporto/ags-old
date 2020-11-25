﻿using AGS.Types;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.InteropServices;

namespace AGS.Editor
{
    public static class BitmapExtensions
    {
        /// <summary>
        /// Gives a scaled deep copy of the input image.
        /// </summary>
        /// <param name="bmp">The image to copy and scale.</param>
        /// <param name="width">Scale the image to this width.</param>
        /// <param name="height">Scale the image to this height.</param>
        /// <returns>A scaled deep copy of the input image.</returns>
        public static Bitmap Scale(this Bitmap bmp, int width, int height) => bmp.Mutate(width, height, g =>
        {
            g.DrawImage(bmp, 0, 0, width, height);
        });

        /// <summary>
        /// Gives back a deep copy of the bitmap with a drawn line.
        /// </summary>
        /// <param name="bmp">The bitmap we want to copy and draw on.</param>
        /// <param name="p1">The starting point for the line.</param>
        /// <param name="p2">The end point for the line.</param>
        /// <param name="color">The color of the line.</param>
        /// <param name="scale">Adjust coordinates for the input scale.</param>
        /// <returns>A new bitmap with a line drawn on it.</returns>
        public static Bitmap DrawLine(this Bitmap bmp, Point p1, Point p2, Color color, double scale = 1.0) => bmp.Mutate(g =>
        {
            p1 = new Point((int)(p1.X * scale), (int)(p1.Y * scale));
            p2 = new Point((int)(p2.X * scale), (int)(p2.Y * scale));

            g.DrawImage(bmp, 0, 0);
            g.DrawLine(new Pen(color), p1, p2);
        });

        /// <summary>
        /// Gives back a deep copy of the bitmap with a filled rectangle.
        /// </summary>
        /// <param name="bmp">The bitmap we to copy and draw on.</param>
        /// <param name="p1">The starting point of the rectangle.</param>
        /// <param name="p2">The end point of the rectangle.</param>
        /// <param name="color">The color of the rectangle.</param>
        /// <param name="scale">Adjust coordinates for the input scale.</param>
        /// <returns></returns>
        public static Bitmap FillRectangle(this Bitmap bmp, Point p1, Point p2, Color color, double scale = 1.0) => bmp.Mutate(g =>
        {
            Point origin = new Point(p1.X < p2.X ? p1.X : p2.X, p1.Y < p2.Y ? p1.Y : p2.Y);
            Point originScaled = new Point((int)(origin.X * scale), (int)(origin.Y * scale));

            Size size = new Size(Math.Abs(p1.X - p2.X), Math.Abs(p1.Y - p2.Y));
            Size sizeScaled = new Size((int)(size.Width * scale), (int)(size.Height * scale));

            g.DrawImage(bmp, 0, 0);
            g.FillRectangle(new SolidBrush(color), new Rectangle(originScaled, sizeScaled));
        });

        /// <summary>
        /// Gives back a deep copy of the bitmap with the area matching the position color, filled to
        /// mathc the input color, like a paint bucket.
        /// </summary>
        /// <param name="bmp">The bitmap to copy and draw on.</param>
        /// <param name="position">The position we want to fill from.</param>
        /// <param name="color">The color to use for the filling.</param>
        /// <returns>A new bitmap with the area filled.</returns>
        public static Bitmap FillArea(this Bitmap bmp, Point position, int color, double scale)
        {
            Bitmap result = new Bitmap(bmp.Width, bmp.Height, bmp.PixelFormat);
            Point positionScaled = new Point((int)(position.X * scale), (int)(position.Y * scale));

            byte[] rawData = bmp.GetRawData(bmp.PixelFormat);
            FloodFillImage(rawData, positionScaled, bmp.Size, rawData[(positionScaled.Y * bmp.Width) + positionScaled.X], (byte)color);
            result.SetRawData(rawData, bmp.PixelFormat);

            return result;
        }

        /// <summary>
        /// Makes a deep copy of the bitmap that we can perform drawing operations on, and preserve the pixel format.
        /// </summary>
        /// <param name="bmp">The bitmap to deep copy.</param>
        /// <param name="mutate">The drawable <see cref="Graphics"/> in a lambda.</param>
        /// <returns>A deep copy of the bitmap with the executed drawing operations.</returns>
        public static Bitmap Mutate(this Bitmap bmp, Action<Graphics> mutate) => bmp.Mutate(bmp.Width, bmp.Height, mutate);

        /// <summary>
        /// Makes a deep copy of the bitmap that we can perform drawing operations on, and preserve the pixel format.
        /// </summary>
        /// <param name="bmp">The bitmap to deep copy.</param>
        /// <param name="width">The width of the resulting bitmap.</param>
        /// <param name="height">The height of the resulting bitmap.</param>
        /// <param name="mutate">The drawable <see cref="Graphics"/> in a lambda.</param>
        /// <returns>A deep copy of the bitmap with the executed drawing operations.</returns>
        public static Bitmap Mutate(this Bitmap bmp, int width, int height, Action<Graphics> mutate)
        {
            if (width <= 0) throw new ArgumentException("Scale factor must be greater than 0.", nameof(width));
            if (height <= 0) throw new ArgumentException("Scale factor must be greater than 0.", nameof(height));

            // Drawing operations is not supported on 8-bit images so we have to create a default 32-bit image buffer for drawing
            using (Bitmap drawBuffer = new Bitmap(width, height))
            {
                using (Graphics graphics = Graphics.FromImage(drawBuffer))
                {
                    graphics.CompositingMode = CompositingMode.SourceCopy;
                    graphics.InterpolationMode = InterpolationMode.NearestNeighbor;
                    mutate(graphics);
                }

                Bitmap result = new Bitmap(width, height, bmp.PixelFormat);

                // Make sure we use the correct palette for indexed images
                if (bmp.PixelFormat == PixelFormat.Format8bppIndexed)
                {
                    result.SetPaletteFromGlobalPalette();
                    drawBuffer.Palette = result.Palette;
                }

                // Write drawing buffer to the result image and retain the pixel format.
                result.SetRawData(drawBuffer.GetRawData(bmp.PixelFormat), bmp.PixelFormat);
                return result;
            }
        }

        /// <summary>
        /// Gets the raw data from the bitmap as a byte array.
        /// </summary>
        /// <param name="bmp">The image to get the raw data from.</param>
        /// <returns>The raw data as a byte array.</returns>
        public static byte[] GetRawData(this Bitmap bmp) => bmp.GetRawData(bmp.PixelFormat);

        /// <summary>
        /// Gets the raw data from the bitmap as a byte array.
        /// </summary>
        /// <param name="bmp">The image to get the raw data from.</param>
        /// <param name="pixelFormat">The pixel format to read the raw data as.</param>
        /// <returns>The raw data as a byte array.</returns>
        public static byte[] GetRawData(this Bitmap bmp, PixelFormat pixelFormat)
        {
            BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.ReadOnly, pixelFormat);
            int pixelCount = Math.Abs(data.Stride) * data.Height;
            byte[] bitmapRawData = new byte[pixelCount];
            Marshal.Copy(data.Scan0, bitmapRawData, 0, pixelCount);
            bmp.UnlockBits(data);

            return bitmapRawData;
        }

        /// <summary>
        /// Sets the raw data from the byte array to the bitmap.
        /// </summary>
        /// <param name="bmp">The bitmap to set the raw data to.</param>
        /// <param name="rawData">The raw data to set into the bitmap.</param>
        /// <param name="pixelFormat">The pixel format to read the raw data as.</param>
        public static void SetRawData(this Bitmap bmp, byte[] rawData, PixelFormat pixelFormat)
        {
            BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.WriteOnly, pixelFormat);
            int pixelCount = Math.Abs(data.Stride) * data.Height;
            Marshal.Copy(rawData, 0, data.Scan0, pixelCount);
            bmp.UnlockBits(data);
        }

        /// <summary>
        /// Sets the raw data from the byte array to the bitmap.
        /// </summary>
        /// <param name="bmp">The bitmap to set the raw data to.</param>
        /// <param name="rawData">The raw data to set into the bitmap.</param>
        public static void SetRawData(this Bitmap bmp, byte[] rawData) => bmp.SetRawData(rawData, bmp.PixelFormat);

        /// <summary>
        /// Sets the current game's global palette to a bitmap.
        /// </summary>
        /// <param name="bmp">The bitmap to set to global palette to.</param>
        public static void SetPaletteFromGlobalPalette(this Bitmap bmp)
        {
            ColorPalette palette = bmp.Palette;

            foreach (PaletteEntry global in Factory.AGSEditor.CurrentGame.Palette)
            {
                palette.Entries[global.Index] = global.Colour;
            }

            bmp.Palette = palette; // Get Bitmap.Palette is deep copy so we need to set it back
        }

        /// <summary>
        /// Checks if the x and y coordinate is within the image.
        /// </summary>
        /// <param name="position">The position to check against.</param>
        /// <returns>True if the position is inside the image. False if outside.</returns>
        public static bool Intersects(this Bitmap bmp, Point position)
        {
            return position.X >= 0 && position.X < bmp.Width && position.Y >= 0 && position.Y < bmp.Height;
        }

        /// <summary>
        /// Flood fill algorithm with for-loop instead of recursion to avoid stack overflow issues.
        /// </summary>
        /// <param name="image">The raw byte array of the image.</param>
        /// <param name="position">The starting position for the fill.</param>
        /// <param name="size">The dimensions of the image.</param>
        /// <param name="initial">The color of the starting position.</param>
        /// <param name="replacement">The color to replace the pixels with.</param>
        private static void FloodFillImage(byte[] image, Point position, Size size, byte initial, byte replacement)
        {
            Queue<Point> queue = new Queue<Point>();
            queue.Enqueue(position);

            while (queue.Any())
            {
                position = queue.Dequeue();

                if (position.X >= 0 && position.X < size.Width && position.Y >= 0 && position.Y < size.Height)
                {
                    int i = (position.Y * size.Width) + position.X;

                    if (image[i] == initial)
                    {
                        image[i] = replacement;

                        queue.Enqueue(new Point(position.X + 1, position.Y));
                        queue.Enqueue(new Point(position.X - 1, position.Y));
                        queue.Enqueue(new Point(position.X, position.Y + 1));
                        queue.Enqueue(new Point(position.X, position.Y - 1));
                    }
                }
            }
        }
    }
}