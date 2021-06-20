# importing google_images_download module
from google_images_download import google_images_download

# creating object
response = google_images_download.googleimagesdownload()

search_queries = ['unconscious person floor', 'lying on the floor', 'unconscious person']


def downloadimages(query):
    # keywords is the search query
    # format is the image file format
    # limit is the number of images to be downloaded
    # print urs is to print the image file url
    # size is the image size which can
    # be specified manually ("large, medium, icon")
    # aspect ratio denotes the height width ratio
    # of images to download. ("tall, square, wide, panoramic")
    arguments = {"keywords": query,
                 "format": "jpg",
                 "limit": 1000,
                 "print_urls": True,
                 "size": "large",
                 "output_directory": "f:/dataset",
                 "type": "photo",
                 "related_images": True,
                 "chromedriver": r"C:\Users\User\AppData\Roaming\SPB_Data\.wdm\drivers\chromedriver\win32\90.0.4430.24\chromedriver.exe"
                 }
    try:
        response.download(arguments)

    # Handling File NotFound Error
    except FileNotFoundError:
        arguments = {"keywords": query,
                     "format": "jpg",
                     "limit": 1000,
                     "print_urls": True,
                     "size": "large",
                     "output_directory": "f:/dataset",
                     "type": "photo",
                     "related_images": True,
                     "chromedriver": r"C:\Users\User\AppData\Roaming\SPB_Data\.wdm\drivers\chromedriver\win32\90.0.4430.24\chromedriver.exe"
                     }

        # Providing arguments for the searched query
        try:
            # Downloading the photos based
            # on the given arguments
            response.download(arguments)
        except:
            pass


# Driver Code
for query in search_queries:
    downloadimages(query)
    print()
