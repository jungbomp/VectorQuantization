# Vector Quantization for Image Compression

This is an implementation of [vector quantization](https://en.wikipedia.org/wiki/Vector_quantization) for image compression on Win32API(C++). This generates a compressed image corrresponding to a input image. I added vector quantization module to this which is based on basic starter code which draw image on the screen.


### Vector Quantization

Vector Quantization chooses best N vectors to express an image. Assume the size of vector is two. In this case, two pixel values of an image represent a vector. If the image is gray scale, each pixel has a range from 0 to 255. Thus, a vector contains a pair of a byte value. Let's say, an arbitrary two vectors contains (135, 240) and (135, 241). The latter can be replaced by the former since two vectors are enough similar. So, the code book contains former vector and assigns index to it. The image uses indices to express whole image. Finally, the image is compressed based on the code book. Therefore, Vector Quantization chooses best N vector to express an image. The less the N is, the higher compression rate the image achieves. I used [K-Means Clustering](https://en.wikipedia.org/wiki/K-means_clustering) to choose best N vector.


![Authoring tool sample screen shot](screenshot01.png)

![Playing tool sample screen shot](screenshot02.png)

The repository includes:
* Source code
* Sample Media contents
* Sample screen shots

### Datasets

The media contents consist of series of image files. Each image file is .RGB where the resolution is 352x288 containing 352x288 red bytes, followed by 352x288 green bytes, floowed by 352x288 blue bytes.

Autio Format is .WAV which is 16 bits per sample, smpling rate of 44,100 samples per second.

The tool plays link metafile which is json file containing origin of hyperlink and destination. Once link area is selected on a frame, it jumps to linked video and play.

The link of dataset
* [AlFilm](https://drive.google.com/file/d/1X7xJV0em3uiRn05Y-B5pRDPVkAx1BsgN/view?usp=sharing)
* [London](https://drive.google.com/file/d/1asHib_JR-xik9FylzS-uzvGARFbTJl7c/view?usp=sharing)
* [NewYorkCity](https://drive.google.com/file/d/1B8WbMcsKiyJFV5erhpVQDUVuAIRqp1Uk/view?usp=sharing)
* [USC](https://drive.google.com/file/d/1XjmxxeHgBIZb_uMnKP2U9k_E8ejTk9Do/view?usp=sharing)

### Demo
![Demo](demo.gif)


### Status

This is final project of CSCI-576 Multimedia System Design, 2018 fall

Version 1.0

