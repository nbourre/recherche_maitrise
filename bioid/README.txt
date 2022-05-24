The BioID Face Database
=======================

http://www.humanscan.de/support/downloads/facedb.php

Files
=====

BioID-FD-Eyepos-V1.2.zip       Corresponding Eye-Positions files
BioID-FaceDatabase-V1.2.zip    The BioID Face Database itself
bioid_pts.zip                  FGnet Markup Scheme of the BioID Face Database


Description
===========

The BioID Face Database has been recorded and is published to give all
researchers working in the area of face detection the possibility to
compare the quality of their face detection algorithms with
others. During the recording special emphasis has been laid on "real
world" conditions. Therefore the testset features a large variety of
illumination, background and face size.

The dataset consists of 1521 gray level images with a resolution of
384x286 pixel. Each one shows the frontal view of a face of one out of
23 different test persons. For comparison reasons the set also
contains manually set eye postions. The images are labeled
"BioID_xxxx.pgm" where the characters xxxx are replaced by the index
of the current image (with leading zeros). Similar to this, the files
"BioID_xxxx.eye" contain the eye positions for the corresponding
images.

Image File Format
=================

The images are stored in single files using the portable gray map
(pgm) data format. A pgm file contains a data header followed by the
image data. In our case the header consists of four lines of text.

* The first line describes the format of the image data
 (ASCII/binary). In our files the text "P5" indicates that the data
 is written in binary form

* The second line contains the image width written in text form 

* The third line keeps the image height also in text form 

* The fourth line contains the maximum allowed gray value (255 in our images)

The header is followed by a data block containing the image data. The
data is stored line per line from top to bottom using one byte per
pixel.

Eye Position File Format
========================

The eye position files are text files containing a single comment line
followed by the x and the y coordinate of the left eye and the x and
the y coordinate of the right eye separated by spaces. Note that we
refer to the left eye as the person's left eye. Therefore, when
captured by a camera, the position of the left eye is on the image's
right and vice versa.

FGnet Markup Scheme of the BioID Face Database
==============================================

The BioID Face Database is being used within the FGnet project of the
European Working Group on face and gesture recognition. David
Cristinacce and Kola Babalola, PhD students from the department of
Imaging Science and Biomedical Engineering at the University of
Manchester marked up the images from the BioID Face Database. They
selected several additional feature points, which are very useful for
facial analysis and gesture recognition.


For more information about the BioID Face Database, sample pictures
and a sample of the FGnet Markup Scheme please visit

http://www.humanscan.de/support/downloads/facedb.php

For more Information about the FGnet Face and Gesture Recognition
Working group please visit

http://www-prima.inrialpes.fr/FGnet/html/home.html

------------------------------------------------------------
09/05/2003  info@humanscan.de 
