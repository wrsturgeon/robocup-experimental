## Wasserstein

AFAIK a new technique in computer vision.

Seeks to find the difference between two images
not only in terms of intensity differences
but, more importantly, by differences in positional alignment.

E.g., if we have a two very sharp identical images that are a few pixels off,
a normal CV criterion would consider them almost entirely different images;
this technique aims not only to consider them nearly identical
but also to make differentiable any difference that does exist,
such that the gradient of this distance represents the path to alignment.

Note that efficiency is prioritized over instantaneous accuracy:
the methods used here assume we're getting a constant flow of frames.
