Arquitetura de código escolhida: Modular Architecture

Razão: Ter as principais interfaces públicas para a implementacao .c dos módulos acessar sem restrições estruturais dá maior facilidade a adição de novos módulos futuros. Os contratos públicos será a cola entre os módulos.

Contras: Possível dependência circular necessitando refatoração das interfaces
Outras possíveis: Layer, Event, Unix

