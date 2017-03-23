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

## Dataset
The modified PubMed dataset that is used in this project is publicly available at:

[http://academictorrents.com/details/371a9244d2e9344a196a449f898e0a4385b6b43a](http://academictorrents.com/details/371a9244d2e9344a196a449f898e0a4385b6b43a)

By using this collection and indexing it by using the configuration file in [this link](https://github.com/teanalab/MRF-L/tree/master/index), you can replicate results for different runs from indexing to retrieval. 

## Testing
In order to test the runs, you can simply use (IndriRunQuery)[https://sourceforge.net/p/lemur/wiki/IndriRunQuery/]. For example, to test wsuirdaa:

`IndriRunQuery configs/wsuirdaa.cfg > runs/wsuirdaa_new.res`

and you can use (trec_eval)[http://trec.nist.gov/trec_eval/] to obtain the evaluation results:

`trec_eval qrels runs/wsuirdaa_new.res`

where `qrels` can be downloaded from (this link)[http://www.trec-cds.org/2016.html].

## Citing MRF-L

When citing MRF-L in academic papers and theses, please use this BibTeX entry:

    @inproceedings{balaneshinkordan2015wsu,
    title={WSU-IR at TREC 2015 Clinical Decision Support Track: Joint Weighting of Explicit and Latent Medical Query Concepts from Diverse Sources.},
    author={Balaneshinkordan, Saeid and Kotov, Alexander and Xisto, Railan},
    booktitle={TREC},
    year={2015}
    }

## Related Paper:
Balaneshin-kordan, Saeid, and Alexander Kotov. "Optimization method for weighting explicit and latent concepts in clinical decision support queries." In Proceedings of the 2016 ACM on International Conference on the Theory of Information Retrieval, pp. 241-250. ACM, 2016. [\[pdf\]](http://www.cs.wayne.edu/kotov/docs/balaneshinkordan-ictir16-full.pdf)
