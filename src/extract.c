/**
 * Copyright (C) 2023 Yanderemine54
 * 
 * This file is part of npkg.
 * 
 * npkg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * npkg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with npkg.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <stdint.h>

#include <archive.h>

#include "extract.h"

void extract_package(const char* filename) {
    struct archive *archive;
    struct archive *decompressedArchive;
    struct archive_entry *archive_entry;
    int result;

    archive = archive_read_new();
    decompressedArchive = archive_write_disk_new();
    archive_write_disk_set_options(decompressedArchive, 0);

    archive_read_support_filter_zstd(archive);
    archive_read_support_format_gnutar(archive);
    archive_read_open_filename(archive, filename, 10240);
    for (;;) {
        result = archive_read_next_header(archive, &archive_entry);
        if (result == ARCHIVE_EOF)
            break;
        result = archive_write_header(decompressedArchive, archive_entry);
        copy_data(archive, decompressedArchive);
        result = archive_write_finish_entry(decompressedArchive);
    }

    archive_read_close(archive);
    archive_read_free(archive);

    archive_write_close(decompressedArchive);
    archive_write_free(decompressedArchive);
}

int copy_data(struct archive* readArchive, struct archive* writeArchive) {
    int result;
    const void* buffer;
    size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
    int64_t offset;
#else
    off_t offset;
#endif

    for (;;) {
        result = archive_read_data_block(readArchive, &buffer, &size, &offset);
        if (result == ARCHIVE_EOF)
            return ARCHIVE_OK;
        if (result != ARCHIVE_OK)
            return result;
        result = archive_write_data_block(writeArchive, buffer, size, offset);
        if (result != ARCHIVE_OK)
            return result;
    }
}