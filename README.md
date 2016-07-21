# MRF-L

## Competition:
[TREC 2015 CDS track](http://www.trec-cds.org/2015.html)

Group Name: WSU-IR 

## Corresponding Paper:
Balaneshin-kordan, S., Kotov, A., and Xisto, R. (2015). WSU-IR at TREC 2015 Clinical Decision Support
Track: Joint Weighting of Explicit and Latent Medical Query Concepts from Diverse Sources. In Proceedings
of the 2015 Text Retrieval Conference. [\[pdf\]](http://trec.nist.gov/pubs/trec24/papers/wsu_ir-CL.pdf)

## Paper Abstract:
This paper describes participation of WSU-IR group in TREC
2015 Clinical Decision Support (CDS) track. We present a Markov Random
Fields-based retrieval model and an optimization method for jointly
weighting statistical and semantic unigram, bigram and multi-phrase
concepts from the query and PRF documents as well as three specific
instantiations of this model that we used to obtain the runs submitted
for each task in this track. These instantiations consider different types
of concepts and use different parts of topics as queries.

## Description:
This program is a part of Project INTGR. Project INTGR aims to integrate semantic and statistical query transformation approaches by adopting optimization techniques. The paper for INTGR project is published in ICTIR\`16.
The INTGR project focuses on medical domain information retrieval systems in which the queries tend to be verbose. It aims to integrate a sequential dependency model in describing queries that are transformed by using the concepts from different semantic and statistic concept sources.

## Concept Sources
1. unigram concepts from query,
2. unigram concepts from top-documents,
3. UMLS n-gram concepts from query,
4. n-gram concepts from Google search results.

Concepts from the first three sources are extracted automatically, while the concepts from the last source is extracted manually. A computer science undergraduate student was asked to choose 2-3 concepts from top-10 Google search results such that they satisfy the following two conditions: 

1. They are related to medical domain (e.g., "blood cancer")
2. They are not very common phrases (e.g., "health care")

All experiments were conducted using Indri 5.8. The .cfg files are the configuation files of [IndriRunQuery](http://sourceforge.net/p/lemur/wiki/IndriRunQuery/). All the runs submitted for TREC-CDS15 can be reproduced by using these configuration files.
