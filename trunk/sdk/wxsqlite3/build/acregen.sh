#!/bin/sh
#
# Author: Francesco Montorsi
# RCS-ID: $Id: acregen.sh,v 1.2 2005/11/12 23:00:21 utelle Exp $
# Creation date: 14/9/2005
#
# A simple script to generate the configure script for a wxCode component


( aclocal && autoconf && mv configure .. )
