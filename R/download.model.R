download.model <- function(model, base_directory=getwd()){
    base_url <- "http://feature.stanford.edu/webfeature/models"
    pos_file <- paste0(model, ".pos.ff.gz")
    neg_file <- paste0(model, ".neg.ff.gz")

    if(!file.exists(base_directory)) dir.create(base_directory, recursive = TRUE)

    cat("\nDownloading positive feature file...\n")
    download.file(file.path(base_url, model, pos_file),
                  file.path(base_directory, pos_file),
                  quiet=T)
    cat("Downloading negative feature file...\n")
    download.file(file.path(base_url, model, neg_file),
                  file.path(base_directory, neg_file),
                  quiet=T)

    cat("Merging positive and negative .ff.gz files into compressed arff\n")
    .C("ff2arff", model)
    cat(sprintf("Model successfully stored in %s.arff.gz\n", model))

    invisible()
}
