download.model <- function(model, base_directory=getwd()){
    base_url <- "http://feature.stanford.edu/webfeature/models"
    pos_file <- paste0(model, ".pos.ff.gz")
    neg_file <- paste0(model, ".neg.ff.gz")

    if(!file.exists(base_directory)) dir.create(base_directory, recursive = TRUE)

    download.file(paste(base_url, model, pos_file, sep="/"),
                  file.path(base_directory, pos_file))
    download.file(paste(base_url, model, neg_file, sep="/"),
                  file.path(base_directory, neg_file))
}
