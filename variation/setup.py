import setuptools

with open("_README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="roadvariation",
    version="0.1",
    author="Daniel Becker",
    author_email="daniel.becker@ika.rwth-aachen.de",
    description="Vary logical road network input format",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://gitlab.ika.rwth-aachen.de/dbecker/road-generation",
    packages=setuptools.find_packages(),
    install_requires=[
          'sympy',
          'numexpr',
          'numpy',
    ],
    scripts=['bin/variation'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
    include_package_data=True,
)
