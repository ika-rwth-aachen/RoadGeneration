import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="roadnetwork-variation", # Replace with your own username
    version="0.1",
    author="Daniel Becker",
    author_email="daniel.becker@ika.rwth-aachen.de",
    description="Vary logical road network input format",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://gitlab.ika.rwth-aachen.de/dbecker/road-generation",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
)