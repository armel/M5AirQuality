https://www.youtube.com/watch?v=C1U_OXCZ2NE&ab_channel=TheHALProject
https://loader.to/en39/youtube-playlist-downloader.html

----------

ffmpeg -i HAL9000_input.mp4 -vf scale=320:-1 -crf 1 HAL9000_output.mp4
ffmpeg -i HAL9000_output.mp4 -vf "fps=10" -q:v 2 -crf 1 HAL9000.mjpg