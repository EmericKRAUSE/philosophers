/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekrause <emeric.yukii@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 14:32:37 by ekrause           #+#    #+#             */
/*   Updated: 2024/08/19 14:27:25 by ekrause          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	PHILO_H
# define PHILO_H

/**********/
// library

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# define RED "\033[31m"
# define END "\033[0m"

/**********/
// structures

typedef struct s_philo t_philo;
typedef struct s_fork t_fork;

typedef struct s_arg
{
	int		nb_philo;
	int		time_to_die;
	int		time_to_eat;
	int		time_to_sleep;
	int		must_eat;
	int		start;
	t_philo	*philos;
	t_fork	*forks;
}	t_arg;

typedef struct s_fork
{
	int				id;
	pthread_mutex_t	fork;
}	t_fork;

typedef struct s_philo
{
	int			id;
	pthread_t	philo;
	int			last_meal;
	int			nbr_meals;
	t_fork		*left_fork;
	t_fork		*right_fork;
	t_arg		*arg;
}	t_philo;

#endif