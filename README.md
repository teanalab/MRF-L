# MRF-L

This program belongs to the TREC CDS track submission from WSU-IR group in 2015. 

This program is a part of Project INTGR. Project INTGR aims to integrate semantic and statistical query transformation approaches by adopting optimization techniques. This project focuses on medical domain information retrieval systems in which the queries tend to be verbose. This part of the project is submitted to ECIR 2016. It aims to integrate a sequential dependency model in describing queries that are transformed by using the concepts from following sources:

1. unigram concepts from query,
2. unigram concepts from top-documents,
3. UMLS n-gram concepts from query,
4. n-gram concepts from Google search results.

Concepts from the first three sources are extracted automatically, while the concepts from the last source is extracted manually. A computer science undergraduate student was asked to choose 2-3 concepts from top-10 Google search results such that they satisfy the following two conditions: 

1. They are related to medical domain (e.g., "blood cancer")
2. They are not very common phrases (e.g., "health care")

All experiments were conducted using Indri 5.8. The .cfg files are the configuation files of [IndriRunQuery](http://sourceforge.net/p/lemur/wiki/IndriRunQuery/) and all the runs submitted for TREC-CDS15 can be reproduced by using these configuration files.
