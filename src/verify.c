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

#include <stdio.h>

#include <gpgme.h>

#include "verify.h"

int verify_package(gpgme_ctx_t context, const char* signatureFilename, const char* packageFilename) {
    FILE* signatureFile;
    FILE* packageFile;
    gpgme_data_t signature;
    gpgme_data_t package;
    gpgme_verify_result_t result;
    gpgme_signature_t signatureLinkedList;
    gpgme_error_t error;

    signatureFile = fopen(signatureFilename, "rb");
    packageFile = fopen(packageFilename, "rb");

    puts("Streams created...");

    error = gpgme_data_new_from_stream(&signature, signatureFile);
    if (error != GPG_ERR_NO_ERROR) {
        puts("signature error");
        gpg_strerror(error);
    }
    error = gpgme_data_new_from_stream(&package, packageFile);
    if (error != GPG_ERR_NO_ERROR) {
        puts("package error");
        gpg_strerror(error);
    }

    puts("Data buffers created");

    gpgme_data_rewind(signature);

    error = gpgme_op_verify(context, signature, package, NULL);
    if (error != GPG_ERR_NO_ERROR) {
        gpg_strerror(error);
    }
    if (error != GPG_ERR_NO_ERROR) {
        gpg_strerror(error);
    }
    puts("verified without result");

    gpgme_data_rewind(signature);

    result = gpgme_op_verify_result(context);

    puts("Verified package");

    if (!result) {
        puts("result is a NULL pointer! The operation failed for some reason and it didn't tell us why");
        goto bad_signature_cleanup;
    }
    signatureLinkedList = result->signatures;
    do {
        if ((signatureLinkedList->summary & GPGME_SIGSUM_VALID)) {
            puts("Good singature cleanup");
            gpgme_data_release(signature);
            gpgme_data_release(package);
            fclose(signatureFile);
            fclose(packageFile);
            return VERIFY_OK;
        } else {
            puts("Next signature");
            signatureLinkedList = signatureLinkedList->next;
        }
    } while (signatureLinkedList->next != NULL);
bad_signature_cleanup:
    puts("Bad signature cleanup");
    gpgme_data_release(signature);
    gpgme_data_release(package);
    fclose(signatureFile);
    fclose(packageFile);
    return VERIFY_ERR;
}